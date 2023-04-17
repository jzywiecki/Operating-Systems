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
    //handler dla siginta, usuwanie kolejki itd. obsluguje juz atexit
    printf("SIGINT\n");
    exit(EXIT_SUCCESS);
}

void delete_queue(){
    //wywolywane przy atexit, usuwamy tu kolejke i wysylamy wiadomosc do serwera, ze klient zamkniety, aby mogl przyznac id komus innemu
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

void list_handler(){ //handler dla listy, wysylamy wiadomosc z prosba o liste na kolejke shared i oczekujemy na wiadomosc z listą na kolejce klienta
    Message* message = malloc(sizeof(Message));
    message->client_queue_key = client_queue;
    message->client_queue_id = queue_id;
    message->message_type = 3;
    msgsnd(shared_queue_id, message, sizeof(Message), 0);
    msgrcv(queue_id, message, sizeof(Message), 0, 0);
    printf("%s\n", message->message_text);
    free(message);
}

void toall_handler(char *operator){
    //wysylamy wiadomosc do wszystkich innych klientów, beda to slowa oddzielone spacjami (tak jakbysmy normalnie pisali zdanie)
    Message* message = malloc(sizeof(Message));
    message->client_queue_key = client_queue;
    message->client_queue_id = queue_id;
    message->client_id = client_id;
    message->message_type = 2;

    if ((operator = strtok(NULL, sep)) == NULL) {
        printf("Insert message!\n");
    }
    time_t ttime;
    struct tm * currentTime;
    time( & ttime );
    currentTime = localtime( & ttime );
    strcpy(message->message_text, asctime(currentTime)); //tutaj do wiadomosci dodaje czas wyslania, zgodnie z poleceniem
    strcat(message->message_text, operator);
    strcat(message->message_text, " ");
    while ((operator = strtok(NULL, sep)) != NULL){
        strcat(message->message_text, operator);
        strcat(message->message_text, " ");
    }
    msgsnd(shared_queue_id, message, sizeof(Message), 0);
}


void toone_handler(char *operator){
    //wyslanie wiadomosci do konkretnego klienta, ktorego okreslamy jako argument wywolania "2one <other_id> <wiadomosc>"
    Message* message = malloc(sizeof(Message));
    if ((operator = strtok(NULL, sep)) != NULL) { //wydobycie <other_id>
        int a = atoi(operator);
        message->other_id = a;
    }
    else{
        printf("Bad client id!\n");
        return;
    }
    message->client_queue_key = client_queue;
    message->client_queue_id = queue_id;
    message->client_id = client_id;
    message->message_type = 1;
    if ((operator = strtok(NULL, sep)) == NULL) {
        printf("Insert message!\n");
    }
    time_t ttime;
    struct tm * currentTime;
    time( & ttime );
    currentTime = localtime( & ttime );
    strcpy(message->message_text, asctime(currentTime)); //tutaj do wiadomosci dodaje czas wyslania, zgodnie z poleceniem
    strcat(message->message_text, operator);
    strcat(message->message_text, " ");
    while ((operator = strtok(NULL, sep)) != NULL){
        strcat(message->message_text, operator);
        strcat(message->message_text, " ");
    }
    msgsnd(shared_queue_id, message, sizeof(Message), 0); //wyslanie wiadomosci
}

void stop_handler(){
    //stop po prostu wysylamy sigint aby uniknac redundancji kodu, wywola sie handler i potem atexit
    kill(getpid(), SIGINT);
}

void join_to_server(){
    //dolaczanie do serwera, wazna rzecz, musimy wyslac serwerowi wiadomosc, z ktorej odczyta sobie nasza kolejke klienta i przypisze nam client_id
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
    setbuf(stdout, NULL); //bufer wyjscia ustawiam tak, aby wszystko nam sie wypisywalo od razu
    atexit(delete_queue); //przypisuje atexit oraz handler
    signal(SIGINT, sigint_handler);

    char* queues_path = getenv("HOME");

    client_queue = ftok(queues_path, getpid()); //kolejka klienta, uzywamy pid klienta, aby uzyskac unikalny token
    queue_id = msgget(client_queue, IPC_CREAT | 0666);

    key_t shared_queue = ftok(queues_path, PROJECT_NUMBER); //kolejka wspolna
    shared_queue_id = msgget(shared_queue, 0);

    join_to_server(); //dolaczamy do serwera

    char line[LINE_MAX];
    Message* message = malloc(sizeof(Message));
    while(1) {
        sleep(1);
        while (msgrcv(queue_id, message, sizeof(Message), 0, IPC_NOWAIT) != -1) { //tutaj sprawdzamy czy otrzymalismy wiadomosci i je printujemy z czasem otrzymania i wyslania
            time_t ttime;
            struct tm *currentTime;
            time(&ttime);
            currentTime = localtime(&ttime);
            printf("Recieve time:\n%sSend time:\n%s\n", asctime(currentTime), message->message_text);
        }

        fgets(line, LINE_MAX, stdin); //prosty interfejs repl, podobny jak na cw01
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
