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
int client_id = -1;
char* sep = " \n";
key_t client_queue;
int queue_id;
int shared_queue_id;

void sigint_handler(int signum){
    printf("SIGINT\n");
    exit(EXIT_SUCCESS);
}

void delete_queue(){
    if (msgctl(queue_id, IPC_RMID, NULL) < 0){
        printf("Could not delete queue!\n");
    }
    printf("Deleted queue!\n");
    Message* message = malloc(sizeof(Message));
    message->client_queue_key = client_queue;
    message->client_queue_id = queue_id;
    message->message_type = 4;
    msgsnd(shared_queue_id, message, sizeof(Message), 0);
    free(message);
}

void list_handler(){
    Message* message = malloc(sizeof(Message));
    message->client_queue_key = client_queue;
    message->client_queue_id = queue_id;
    message->message_type = 1;
    msgsnd(shared_queue_id, message, sizeof(Message), 0);
    msgrcv(queue_id, message, sizeof(Message), 0, 0);
    printf("%s\n", message->message_text);
    free(message);
}

void toall_handler(char *operator){
    Message* message = malloc(sizeof(Message));
    message->client_queue_key = client_queue;
    message->client_queue_id = queue_id;
    message->client_id = client_id;
    message->message_type = 2;

    operator = strtok(NULL, sep);
    strcpy(message->message_text, operator);
    while ((operator = strtok(NULL, sep)) != NULL){
        strcat(message->message_text, operator);
    }
    msgsnd(shared_queue_id, message, sizeof(Message), 0);
}


void toone_handler(char *operator){
    Message* message = malloc(sizeof(Message));
    operator = strtok(NULL, sep);
    int a = atoi(operator);
    message->other_id = a;

    message->client_queue_key = client_queue;
    message->client_queue_id = queue_id;
    message->client_id = client_id;
    message->message_type = 3;

    operator = strtok(NULL, sep);
    strcpy(message->message_text, operator);
    while ((operator = strtok(NULL, sep)) != NULL){
        strcat(message->message_text, operator);
    }
    msgsnd(shared_queue_id, message, sizeof(Message), 0);
}

void stop_handler(){
    kill(getpid(), SIGINT);
}

void join_to_server(){
    Message* message = malloc(sizeof(Message));
    message->client_queue_key = client_queue;
    message->client_queue_id = queue_id;
    message->message_type = 5;

    msgsnd(shared_queue_id, message, sizeof(Message), 0);
    msgrcv(queue_id, message, sizeof(Message), 0, 0);

    client_id = message->client_id;
    if (client_id == -1){
        printf("Server full!\n");
        exit(0);
    }

    printf("Client registered with ID: %d\n", client_id);
    free(message);
}


int main(){
    setbuf(stdout, NULL);
    atexit(delete_queue);
    signal(SIGINT, sigint_handler);

    char* queues_path = getenv("HOME");

    client_queue = ftok(queues_path, getpid());
    queue_id = msgget(client_queue, IPC_CREAT | 0666);

    key_t shared_queue = ftok(queues_path, PROJECT_NUMBER);
    shared_queue_id = msgget(shared_queue, 0);

    join_to_server();

    char line[LINE_MAX];
    Message* message = malloc(sizeof(Message));
    while(1) {
        sleep(1);
        while (msgrcv(queue_id, message, sizeof(Message), 0, IPC_NOWAIT) != -1) {
            time_t ttime;
            struct tm *currentTime;
            time(&ttime);
            currentTime = localtime(&ttime);
            printf("%s %s\n", asctime(currentTime), message->message_text);
        }

        fgets(line, LINE_MAX, stdin);
        if (strlen(line) == 1) {
            printf("Input correct command!\n");
            continue;
        }
        char *operator = strtok(line, sep);
        if (strcmp(operator, "list") == 0) {
            list_handler();
        } else if (strcmp(operator, "2all") == 0) {
            toall_handler(operator);
        } else if (strcmp(operator, "2one") == 0) {
            toone_handler(operator);
        } else if (strcmp(operator, "stop") == 0) {
            stop_handler();
        } else {
            printf("Command could not be recognised. Check spelling and try again!\n");
        }
    }
}
