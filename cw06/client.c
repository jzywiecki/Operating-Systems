#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/msg.h>
#include <unistd.h>

#include "shared.h"

#define LINE_MAX 255
int queue_id = -1;
int session_id = -1;

void sigint_handler(int signum){
    printf("SIGINT\n");
    exit(EXIT_SUCCESS);
}

void delete_queue(){
    if (msgctl(queue_id, IPC_RMID, NULL) < 0){
        printf("Could not delete queue!\n");
    }
    printf("Deleted queue!\n");
}

void list_handler(key_t client_queue, int queue_id, int shared_queue_id){
    Message* message = malloc(sizeof(Message));
    message->client_queue_key = client_queue;
    message->client_queue_id = queue_id;
    message->message_type = 1;
    msgsnd(shared_queue_id, message, sizeof(Message), 0);
    free(message);
}

void toall_handler(char *operator){

}


void toone_handler(char *operator){

}

void stop_handler(){

}

void join_to_server(key_t client_queue, int queue_id, int shared_queue_id){
    Message* message = malloc(sizeof(Message));
    message->client_queue_key = client_queue;
    message->client_queue_id = queue_id;
    message->message_type = 5;

    msgsnd(shared_queue_id, message, sizeof(Message), 0);
    msgrcv(queue_id, message, sizeof(Message), 0, 0);

    session_id = atoi(message->message_text);
    if (session_id > MAX_CLIENTS){
        printf("Server full!\n");
        exit(0);
    }

    printf("Client registered with ID: %d\n", session_id);
    free(message);
}


int main(){
    setbuf(stdout, NULL);
    atexit(delete_queue);
    signal(SIGINT, sigint_handler);

    char* queues_path = getenv("HOME");

    key_t client_queue = ftok(queues_path, getpid());
    queue_id = msgget(client_queue, IPC_CREAT | 0666);

    key_t shared_queue = ftok(queues_path, PROJECT_NUMBER);
    int shared_queue_id = msgget(shared_queue, 0);

    join_to_server(client_queue, queue_id, shared_queue_id);

    char line[LINE_MAX];
    while(1){
        fgets(line, LINE_MAX, stdin);
        if (strlen(line) == 1){
            printf("Input correct command!\n");
            continue;
        }
        char* operator = strtok(line, " ");
        if (strcmp(operator, "list") == 0){
            list_handler(client_queue, queue_id, shared_queue_id);
        }
        else if (strcmp(operator, "2all") == 0){
            toall_handler(operator);
        }
        else if (strcmp(operator, "2one") == 0){
            toone_handler(operator);
        }
        else if (strcmp(operator, "stop") == 0){
            stop_handler();
        }
        else{
            printf("Command could not be recognised. Check spelling and try again!\n");
        }
    }

}
