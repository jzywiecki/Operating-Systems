#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>


int increment_semaphore(int semid, int flag) {
    struct sembuf sem_op = { 0, 1, flag };
    return semop(semid, &sem_op, 1);
}

int decrement_semaphore(int semid, int flag) {
    struct sembuf sem_op = { 0, -1, flag };
    return semop(semid, &sem_op, 1);
}

char get_hairstyle_to_do(char* queue){
    if (strlen(queue) == 0){
        printf("Kolejka pusta!\n");
        return '\0';
    }
    char byte = queue[0];
    memcpy(queue, queue + 1, strlen(queue) + 1);
    return byte;
}

void hairstyles_push(char *queue, char byte, int waiting_room) {
    if(strlen(queue) == waiting_room) {
        fprintf(stderr, "[ERROR] Failed to push to queue. Queue full.\n");
        return;
    }
    int size = strlen(queue);
    queue[size] = byte;
    queue[size + 1] = '\0';
    return;
}


int main(int argc, char* argv[] ){
    setbuf(stdout, NULL);

    if (argc != 5){
        printf("Wrong number of arguments!\n");
        exit(1);
    }
    int hairdressers = atoi(argv[1]);
    int clients = atoi(argv[2]);
    int chairs = atoi(argv[3]);
    int waiting_room = atoi(argv[4]);

    int shmid;
    char* queue;

    if((shmid = shmget(ftok(getenv("HOME"), 4), waiting_room*sizeof(char), IPC_CREAT | 0666)) == -1){
        perror("shmid");
    };

    queue = shmat(shmid, NULL, 0);

    int sem_queue;
    int sem_hairdressers;
    int sem_chairs;

    key_t key;

    key = ftok(getenv("HOME"), 1);
    sem_queue = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (sem_queue == -1) {
        sem_queue = semget(key, 1, 0);
        semctl(sem_queue, 0, IPC_RMID);
        sem_queue = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    }

    key = ftok(getenv("HOME"), 2);
    sem_hairdressers = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (sem_hairdressers == -1) {
        sem_hairdressers = semget(key, 1, 0);
        semctl(sem_hairdressers, 0, IPC_RMID);
        sem_hairdressers = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    }

    key = ftok(getenv("HOME"), 3);
    sem_chairs = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (sem_chairs == -1) {
        sem_chairs = semget(key, 1, 0);
        semctl(sem_chairs, 0, IPC_RMID);
        sem_chairs = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    }

    semctl(sem_queue, 0, SETVAL, waiting_room);
    semctl(sem_hairdressers, 0, SETVAL, hairdressers);
    semctl(sem_chairs, 0, SETVAL, chairs);



    if (hairdressers < chairs){
        printf("There are not enough hairdressers for chairs!\n");
        exit(1);
    }


    int queue_size = 0;
    while (clients > 0){
        printf("----------------------------------\n");
        pid_t pid = fork();

        int sem_val = semctl(sem_queue, 0, GETVAL);

        if (pid == 0){

            //poczekalnia
//            sem_val = semctl(sem_queue, 0, GETVAL);
//            printf("Semaphore value: %d\n", sem_val);
//            printf("waiting_room: %d\n", waiting_room);


            if (decrement_semaphore(sem_queue, IPC_NOWAIT) == -1){
                printf("[%d] Kolejka pełna. Klient opuszcza kolejke\n", clients);
                exit(1);
            }

            printf("[%d] Klient wszedł do poczekalni!\n", clients);

            srand(time(NULL));
            char randomletter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[rand() % 26];
            printf("[%d] Klient wybrał fryzure %c!\n", clients, randomletter);
            hairstyles_push(queue, randomletter, waiting_room);


            //fryzjerzy
            decrement_semaphore(sem_hairdressers, SEM_UNDO);
            printf("[%d] Fryzjer został dopasowany!\n", clients);

            //krzesła
            decrement_semaphore(sem_chairs, SEM_UNDO);
            printf("[%d] Znaleziono wolne krzesło!\n", clients);

            //fryzura robienie (sleep itd.)
            char hairstyle = get_hairstyle_to_do(queue);
            int num = hairstyle - 'A' + 1;

            //inkrementacja poczekalni, zaczyna sie robienie fryzury
            increment_semaphore(sem_queue, IPC_NOWAIT);
//            sem_val = semctl(sem_queue, 0, GETVAL);
//            printf("Semaphore value: %d\n", sem_val);

            printf("[%d] Fryzjer wyykonuje fryzurę nr %c!\n",clients, hairstyle);
            sleep(num);

            //inkrementacja krzeseł
            increment_semaphore(sem_chairs, SEM_UNDO);
            printf("[%d] Fryzjer skończył wykonywać fryzurę nr %c!\n", clients, hairstyle);

            //inkrementacja fryzjerów
            increment_semaphore(sem_hairdressers, SEM_UNDO);

            exit(0);
        }

        sleep(3);
        clients--;
    }

    while(wait(NULL) > 0){};
    printf("----------------------------------\n");
    printf("Praca została zakończona, klienci z nowymi fryzurami poszli do domu!\n");
    //usuwanie semaforów
    semctl(sem_queue, 0, IPC_RMID);
    semctl(sem_hairdressers, 0, IPC_RMID);
    semctl(sem_chairs, 0, IPC_RMID);

    //zwalnianie pamięci współdzielonej
    if (shmdt(queue) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}