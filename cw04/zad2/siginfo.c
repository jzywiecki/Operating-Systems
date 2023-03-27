#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


void handler(int signum, siginfo_t *info, void *context){ //handler trzyargumentowy
    if(signum == SIGUSR1){ //obsluga trzech przykladowych sygnalow i wypisywanie PID procesu i numeru sygnalu
        printf("SIGNAL SIGUSR1, PID: %d, SIGNAL: %d\n", info->si_pid, signum);
    }
    else if (signum == SIGUSR2){
        printf("SIGNAL SIGUSR2, PID: %d, SIGNAL: %d\n", info->si_pid, signum);
    }
    else{ //obsluga bledow
        printf("SIGNAL NOT SUPPORTED, PID: %d, SIGNAL: %d\n", info->si_pid, signum);
    }
}

int main(){
    struct sigaction act;
    act.sa_sigaction = handler; //zamiast sa_handler, tu mamy trzyargumentowy
    act.sa_flags = SA_SIGINFO; //przypisujemy testowaną flagę
    sigemptyset(&act.sa_mask);

    sigaction(SIGUSR1, &act, NULL); //wskazujemy obslugiwanie sygnallow
    sigaction(SIGUSR2, &act, NULL);

    printf("\nSIGINFO TEST\n");
    raise(SIGUSR1); //wysylamy sygnaly
    raise(SIGUSR2);
    return 0;
}