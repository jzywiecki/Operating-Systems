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
    //handler dla siginta, usuwanie kolejki itd. obsluguje juz atexit
    printf("SIGINT\n");
    exit(EXIT_SUCCESS);
}

int get_client_id(Message* message){
    //funkcja pomocnicza, aby sprawdzic id klienta, potrzebna poniewaz w switchu mamy join_user, gdzie nie ma przyznanego jeszcze id, pewnie daloby sie to zrobic znacznie ładniej
    int client_id = -1;
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (client_queues_id[i] == message->client_queue_id){
            client_id = i;
        }
    }
    return client_id;
}

void delete_queue(){
    //wywolywane przy atexit, usuwamy tu kolejke przy zamknieciu serwera
    if (msgctl(shared_queue, IPC_RMID, NULL) < 0){
        printf("Could not delete queue!\n");
    }
    printf("Deleted queue!\n");
}


void list_handler(int client_queue_id){
    //obsluga wyslania listy klientów w wiadomości
    Message* message = malloc(sizeof(Message));
    message->message_type = 1;
    sprintf(message->message_text, "Clients:\n");

    for (int i = 0; i < MAX_CLIENTS; i++){
        if (client_queues_id[i] >= 0){
            sprintf(message->message_text + strlen(message->message_text), "%d\n", i);
        }
    }
    msgsnd(client_queue_id, message, sizeof(Message), 0);

}

void toall_handler(Message* message){
    //obsluga wyslania wiadomosci do wszystkich
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (client_queues_id[i] >= 0) {
            msgsnd(client_queues_id[i], message, sizeof(Message), 0);
        }
    }
}

void toone_handler(Message* message){
    //obsluga wyslania wiadomosci do konkretnego klienta, zakladamy poprawnosc, nie wysylamy informacji zwrotnej o braku klienta
    if (client_queues_id[message->other_id] >= 0) {
        msgsnd(client_queues_id[message->other_id], message, sizeof(Message), 0);
    }
    else{
        printf("Message from %d to %d aborted!\n", message->client_id, message->other_id);
    }
}

void stop_handler(int client_queue_id){
    //stop klienta, usuwamy jego queue id, w ten sposob zwalniamy miejsce dla nastepnych klientów
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (client_queues_id[i] == client_queue_id){
            client_queues_id[i] = -1;
        }
    }
}


void join_user(Message* message){
    //dolaczenie usera do serwera, dodajemy mu id, a nastepnie wysylamy wiadomosc zwrotną
    int clients_counter = 0;
    while (clients_counter <= MAX_CLIENTS && client_queues_id[clients_counter] > 0){
        clients_counter += 1;
    }
    if (clients_counter >= MAX_CLIENTS){
        printf("Maximum number of clients!\n");
        message->client_id = -1; //-1 oznacza, ze serwer pelny, odbieramy to w kliencie
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
    setbuf(stdout, NULL); //bufer wyjscia ustawiam tak, aby wszystko nam sie wypisywalo od razu
    atexit(delete_queue); //przypisuje atexit oraz handler
    signal(SIGINT, handle_sigint);

    for (int i = 0; i < MAX_CLIENTS; i++){ //ustawiam id klientów na -1
        client_queues_id[i] = -1;
    }

    char* queues_path = getenv("HOME");
    key_t shared_queue_key = ftok(queues_path, PROJECT_NUMBER); //tworzymy kolejke współdzieloną
    shared_queue = msgget(shared_queue_key, IPC_CREAT | 0666);

    Message* message = malloc(sizeof(Message));

    while(1){
        msgrcv(shared_queue, message, sizeof(Message), 0, 0); //otrzymujemy wiadomosc
        printf("Recieved message: %ld\n", message->message_type); //informujemy o tym
        time_t ttime;
        struct tm * currentTime;
        time( & ttime );
        currentTime = localtime( & ttime );
        FILE *file = fopen("log.txt", "a");
        fprintf(file, "%s - ", asctime(currentTime)); //dodajemy czas do logów

        int i = get_client_id(message); //wydobycie id klienta
        switch(message->message_type) {
            case 3:
                list_handler(message->client_queue_id); //wywolywanie handlerów
                 fprintf(file, "LIST from client: %d\n", i); //tworzenie logów
                break;
            case 2:
                toall_handler(message);
                fprintf(file, "2ALL from client: %d\n", i);
                break;
            case 1:
                toone_handler(message);
                fprintf(file, "2ONE from client: %d to client: %d\n", i, message->other_id);
                break;
            case 4:
                stop_handler(message->client_queue_id);
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