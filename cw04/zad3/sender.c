#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

volatile sig_atomic_t received = 0; //zmienne lotne, atomic
volatile sig_atomic_t mode = 0;

void signal_handler(int sig) {
    if (sig == SIGUSR1){
        printf("Catcher finished action!\n");
    }
    if (sig == SIGUSR2){ //jesli otrzymamy potwierdzenie od catchera
        printf("Got confirmation signal from catcher!\n");
        received = 1; //ustawiam received na 1, traktuje go jako bool
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) { //obsluga ilosci argumentów
        printf("Not enough arguments!\n");
        exit(1);
    }

    pid_t catcher_pid = atoi(argv[1]); //wyciagamy z 1 argumentu pid catchera, aby potem moc wysylac mu sygnaly
    int modes_to_send[argc-2]; //tablica z trybami do wyslania
    for (int i = 0; i < argc-2; i++) {
        modes_to_send[i] = atoi(argv[i+2]); //wczytujemy do niej te tryby, ktore nastepnie bedziemy wysylac do naszego catchera
    }

    struct sigaction sa;
    sigset_t mask;
    sigemptyset(&sa.sa_mask); //pusta maska
    sa.sa_handler = signal_handler; //instalujemy handler
    sigaction(SIGUSR1, &sa, NULL); //przypisujemy sa do obslugi SIGUSR1
    sigaction(SIGUSR2, &sa, NULL); //przypisujemy sa do obslugi SIGUSR2

    for (int i = 0; i < argc - 2; i++) { //iterujemy przez wszystkie zadane tryby pracy
        mode = modes_to_send[i];

        printf("-------------------- %d --------------------\n", mode);
        printf("Sending SIGUSR1 to catcher with PID %d, mode %d\n", catcher_pid, mode);

        sigqueue(catcher_pid, SIGUSR1, (union sigval) mode); //wyslanie sygnalu do catchera

        printf("Waiting for signal to be confirmed!\n");

        while (!received) { //do czasu az sygnal potwierdzajacy nie otrzymany
            sigemptyset(&mask);
            sigaddset(&mask, SIGUSR1); //maska z sygnalem SIGUSR1
            sigsuspend(&mask); // odebranie sygnalu oczekujacego, oczekujemy do czasu otrzymania sygnalu
        }

        printf("Done!\n");
        received = 0; //wyzerowanie potwierdzenia (wartosc bool)
    }
    return 0;
}