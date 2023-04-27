#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

#include "shared.h"

static Semaphore queue;
static Semaphore chairs;
static Semaphore hairdressers;

int main(int argc, char* argv[]){
    setbuf(stdout, NULL);
    if (argc != 5){
        printf("Wrong number of arguments!\n");
        exit(1);
    }




    char* shared_memory;
//
//    int key = ftok(getenv("HOME"), 0); //kolejka klienta, uzywamy pid klienta, aby uzyskac unikalny token
//    queue_id = shmget(key, , IPC_CREAT | 0666);


    //otworzenie semaforów

    int hairdressers = atoi(argv[1]);
    int clients = atoi(argv[2]);
    int chairs = atoi(argv[3]);
    int waiting_room = atoi(argv[4]);


    if (hairdressers < chairs){
        printf("There is not enough hairdressers for chairs!\n");
        exit(1);
    }

    for (int i = 0; i < hairdressers; i++){
        pid_t pid = fork();

        if (pid == 0){
            execl("./hairdresser", "hairdresser", NULL);
        }
    }
    printf("Hairdressers are ready to work!\n");







    for (int i = 0; i < clients; i++){
        pid_t pid = fork();

        if (pid == 0){
            execl("./client", "client", NULL);
        }
    }
    printf("Clients are ready for awesome haircuts!\n");


    //oczekiwanie na zakonczenie


    //usuwanie pamieci wspolnej

    //zamykanie semaforów


    return 0;
}