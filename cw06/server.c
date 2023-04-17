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
int clients_counter = 0;
int shared_queue;

void handle_sigint(int signum){
    printf("SIGINT\n");
    exit(EXIT_SUCCESS);
}

void delete_queue(){
    if (msgctl(shared_queue, IPC_RMID, NULL) < 0){
        printf("Could not delete queue!\n");
    }
    printf("Deleted queue!\n");
}


void list(){
    printf("Clients:\n");
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (client_queues_id[i] > 0){
            printf("%d\n", i);
        }
    }

}

void toall(){
    return;
}

void toone(){
    return;
}

void stop(){
    return;
}

void join_user(Message* message){
    if (clients_counter >= MAX_CLIENTS){
        printf("Maximum number of clients!\n");
        sprintf(message->message_text, "%d", -1);
    }
    else{
        while (clients_counter <= MAX_CLIENTS && client_queues_id[clients_counter] > 0){
            clients_counter += 1;
        }
        sprintf(message->message_text, "%d", clients_counter);
    }
    client_queues_id[clients_counter] = message->client_queue_id;
    client_queues_keys[clients_counter] = message->client_queue_key;
    msgsnd(client_queues_id[clients_counter], message, sizeof(Message), 0);
    printf("Succesfully connected client with id: %d\n", clients_counter);
    clients_counter += 1;
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
        switch(message->message_type) {
            case 1:
                list();
                break;
            case 2:
                toall();
                break;
            case 3:
                toone();
                break;
            case 4:
                stop();
                break;
            case 5:
                join_user(message);
                break;
            default:
                printf("Not known message!\n");
                break;
        }
    }

    return 0;

}