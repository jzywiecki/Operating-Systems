#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/msg.h>

#include "shared.h"

int client_queues_id[MAX_CLIENTS];
key_t client_queues_keys[MAX_CLIENTS];
int shared_queue;

void handle_sigint(int signum){
    printf("SIGINT\n");
    exit(EXIT_SUCCESS);
}

int get_client_id(Message* message){
    int client_id = -1;
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (client_queues_id[i] == message->client_queue_id){
            client_id = i;
        }
    }
    return client_id;
}

void delete_queue(){
    if (msgctl(shared_queue, IPC_RMID, NULL) < 0){
        printf("Could not delete queue!\n");
    }
    printf("Deleted queue!\n");
}


void list(int client_queue_id){
    Message* message = malloc(sizeof(Message));
    sprintf(message->message_text, "Clients:\n");

    for (int i = 0; i < MAX_CLIENTS; i++){
        if (client_queues_id[i] >= 0){
            sprintf(message->message_text + strlen(message->message_text), "%d\n", i);
        }
    }
    message->message_type = 1;
    msgsnd(client_queue_id, message, sizeof(Message), 0);

}

void toall(Message* message){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (client_queues_id[i] >= 0) {
            msgsnd(client_queues_id[i], message, sizeof(Message), 0);
        }
    }
}

void toone(Message* message){
    if (client_queues_id[message->other_id] >= 0) {
        msgsnd(client_queues_id[message->other_id], message, sizeof(Message), 0);
    }
    else{
        printf("Message from %d to %d aborted!\n", message->client_id, message->other_id);
    }
}

void stop(int client_queue_id){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (client_queues_id[i] == client_queue_id){
            client_queues_id[i] = -1;
        }
    }
}


void join_user(Message* message){
    int clients_counter = 0;
    while (clients_counter <= MAX_CLIENTS && client_queues_id[clients_counter] > 0){
        clients_counter += 1;
    }
    if (clients_counter >= MAX_CLIENTS){
        printf("Maximum number of clients!\n");
        message->client_id = -1;
    }
    else{
        message->client_id = clients_counter;
    }
    client_queues_id[clients_counter] = message->client_queue_id;
    client_queues_keys[clients_counter] = message->client_queue_key;
    msgsnd(client_queues_id[clients_counter], message, sizeof(Message), 0);
    printf("Succesfully connected client with id: %d\n", clients_counter);
}


int main(){
    setbuf(stdout, NULL);
    atexit(delete_queue);
    signal(SIGINT, handle_sigint);

    for (int i = 0; i < MAX_CLIENTS; i++){
        client_queues_id[i] = -1;
    }

    char* queues_path = getenv("HOME");
    key_t shared_queue_key = ftok(queues_path, PROJECT_NUMBER);
    shared_queue = msgget(shared_queue_key, IPC_CREAT | 0666);

    Message* message = malloc(sizeof(Message));

    while(1){
        msgrcv(shared_queue, message, sizeof(Message), 0, 0);
        printf("Recieved message: %ld\n", message->message_type);
        time_t ttime;
        struct tm * currentTime;
        time( & ttime );
        currentTime = localtime( & ttime );
        FILE *file = fopen("log.txt", "a");
        fprintf(file, "%s - ", asctime(currentTime));

        int i = get_client_id(message);
        switch(message->message_type) {
            case 1:
                list(message->client_queue_id);
                 fprintf(file, "LIST from client: %d\n", i);
                break;
            case 2:
                toall(message);
                fprintf(file, "2ALL from client: %d\n", i);
                break;
            case 3:
                toone(message);
                fprintf(file, "2ONE from client: %d to client: %d\n", i, message->other_id);
                break;
            case 4:
                stop(message->client_queue_id);
                fprintf(file, "STOP from client: %d\n", i);
                break;
            case 5:
                join_user(message);
                int i = get_client_id(message);
                fprintf(file, "JOINED client: %d\n", i);
                break;
            default:
                printf("Not known message!\n");
                fprintf(file, "UNKNOWN from client: %d\n", i);
                break;
        }
        fclose(file);
    }
    return 0;
}