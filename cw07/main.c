#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>


int increment_semaphore(int semid, int flag) {
    //zwiększenie podanego semafora o 1, dodajemy również możliwość dodania flagi
    struct sembuf sem_op = { 0, 1, flag };
    return semop(semid, &sem_op, 1);
}

int decrement_semaphore(int semid, int flag) {
    //zmniejszenie podanego semafora o 1, dodajemy również możliwość dodania flagi
    struct sembuf sem_op = { 0, -1, flag };
    return semop(semid, &sem_op, 1);
}

char get_hairstyle_to_do(char* queue){
    //pobranie 1 elementu z kolejki
    if (strlen(queue) == 0){ //jeśli kolejka pusta
        printf("Kolejka pusta!\n");
        return '\0';
    }
    char byte = queue[0]; //pobieramy 1 fryzurę
    memcpy(queue, queue + 1, strlen(queue) + 1); //przesuwamy pozostałe fryzury
    return byte;
}

void hairstyles_push(char *queue, char byte, int waiting_room) {
    //dodanie na koniec kolejki fryzury
    if(strlen(queue) == waiting_room) { //jesli pelna
        printf("Kolejka pełna, nie można dodać elementu.\n");
        return;
    }
    queue[strlen(queue)] = byte; //dodanie fryzury
    queue[strlen(queue) + 1] = '\0'; //dodanie konca napisu
    return;
}

int main(int argc, char* argv[] ){
    setbuf(stdout, NULL); //ustawiam bufor wyjscia na NULL zeby od razu wypisywac komunikaty

    if (argc != 5){
        printf("Nieprawidłowa liczba argumentów!\n");
        exit(1);
    }
    int hairdressers = atoi(argv[1]); //przypisujemy argumenty wejscia do zmiennych dla wygody
    int clients = atoi(argv[2]);
    int chairs = atoi(argv[3]);
    int waiting_room = atoi(argv[4]);

    int shmid; //id pamieci wspoldzielonej
    char* queue;

    if((shmid = shmget(ftok(getenv("HOME"), 4), waiting_room*sizeof(char), IPC_CREAT | 0666)) == -1){
        perror("shmid");
    };

    queue = shmat(shmid, NULL, 0); //przypisujemy naszą kolejkę

    int sem_queue; //semafory
    int sem_hairdressers;
    int sem_chairs;
    int sem_shared_memory;

    key_t key;

    key = ftok(getenv("HOME"), 1); //generujemy klucze dla semaforów
    sem_queue = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666); //tworzymy semafor
    if (sem_queue == -1) { //gdy bedzie blad
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

    key = ftok(getenv("HOME"), 5);
    sem_shared_memory = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (sem_shared_memory == -1) {
        sem_shared_memory = semget(key, 1, 0);
        semctl(sem_shared_memory, 0, IPC_RMID);
        sem_shared_memory = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    }

    //ustawiamy wartosci semaforow
    semctl(sem_queue, 0, SETVAL, waiting_room);
    semctl(sem_hairdressers, 0, SETVAL, hairdressers);
    semctl(sem_chairs, 0, SETVAL, chairs);
    semctl(sem_shared_memory, 0, SETVAL, 1);

    if (hairdressers < chairs){ //warunek z zadania, ze fryzjerow ma byc conajmniej tyle co krzesel
        printf("Nie ma wystarczająco fryzjerów, jest ich mniej niż krzeseł!\n");
        exit(1);
    }

    while (clients > 0){ //tyle ile klientow
        printf("----------------------------------\n");
        pid_t pid = fork(); //forkujemy

        if (pid == 0){ //jako dziecko
            //sprawdzamy poczekalnie, flaga IPC_NOWAIT sprawia, ze jesli jest pelna to po prostu klient ją opuści
            if (decrement_semaphore(sem_queue, IPC_NOWAIT) == -1){
                printf("[%d] Kolejka pełna. Klient opuszcza kolejke\n", getpid());
                exit(1); //opuszczenie klientem
            }

            printf("[%d] Klient wszedł do poczekalni!\n", getpid()); //klient jest w poczekalni, teraz krzeslo i fryzjer

            srand(time(NULL));
            //fryzury oznaczam literami, w ten sposob mozna zobaczyc zachowaną kolejnosc wykonywania fryzur
            char randomletter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[rand() % 26];
            printf("[%d] Klient wybrał fryzure %c!\n", getpid(), randomletter); //informuje jaką klient wybral fryzure

            //uzywam semafora aby upewnic sie, ze tylko jeden klient korzysta z pamieci wspoldzielonej
            decrement_semaphore(sem_shared_memory, SEM_UNDO);
            hairstyles_push(queue, randomletter, waiting_room); //dodaje fryzure do kolejki wykonywania
            increment_semaphore(sem_shared_memory, SEM_UNDO);


            //fryzjerzy
            decrement_semaphore(sem_hairdressers, SEM_UNDO);
            //dopasowujemy fryzjera, ponownie uzywamy semafora, jesli nie ma fryzjerow klient bedzie czekac w poczekalni
            printf("[%d] Fryzjer został dopasowany!\n", getpid());

            //krzesła
            decrement_semaphore(sem_chairs, SEM_UNDO);
            //dopasowujemy krzeslo, ponownie uzywamy semafora, jesli nie ma krzesel klient bedzie czekac w poczekalni
            printf("[%d] Znaleziono wolne krzesło!\n", getpid());

            //fryzura robienie (sleep itd.)
            //bierzemy fryzure, ponownie zabezpieczam uzywanie pamieci wspoldzielonej
            decrement_semaphore(sem_shared_memory, SEM_UNDO);
            char hairstyle = get_hairstyle_to_do(queue);
            increment_semaphore(sem_shared_memory, SEM_UNDO);

            //czas robienia fryzury okreslam za pomoca numeru ASCII wylosowanej fryzury
            int num = hairstyle - 'A' + 1;

            //inkrementacja poczekalni, zaczyna sie robienie fryzury
            //tutaj klient ma juz przypisane krzeslo, fryzjera, fryzjer wie jaką fryzure bedzie robic i ile czasu to zajmie, zwalnia sie miejsce w poczekalni
            increment_semaphore(sem_queue, IPC_NOWAIT);

            //wykonujemy fryzure
            printf("[%d] Fryzjer wyykonuje fryzurę nr %c!\n",getpid(), hairstyle);
            sleep(num);

            //inkrementacja krzeseł - fryzjer skonczyl prace, krzeslo sie zwalnia
            increment_semaphore(sem_chairs, SEM_UNDO);
            printf("[%d] Fryzjer skończył wykonywać fryzurę nr %c!\n", getpid(), hairstyle);

            //inkrementacja fryzjerów - fryzjer zegna sie z klientem
            increment_semaphore(sem_hairdressers, SEM_UNDO);

            //klient wychodzi
            exit(0);
        }

        //klienci przychodzą co 3 sekundy
        sleep(3);
        clients--;
    }

    //czekamy az wszyscy klienci beda zadowoleni wracac do domu
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


