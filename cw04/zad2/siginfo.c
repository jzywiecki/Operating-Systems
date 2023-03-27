#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


void handler(int signum, siginfo_t *info, void *context){ //handler trzyargumentowy
    if(signum == SIGUSR1){ //obsluga i wypisywanie PID procesu i numeru sygnalu
        printf("SIGNAL SIGUSR1, PID: %d, SIGNAL: %d\n", info->si_pid, signum);
    }
    else{ //obsluga bledow
        printf("SIGNAL NOT SUPPORTED, PID: %d, SIGNAL: %d\n", info->si_pid, signum);
    }
}

int main(){
    struct sigaction sa;
    sa.sa_sigaction = handler; //zamiast sa_handler, tu mamy trzyargumentowy
    sa.sa_flags = SA_SIGINFO; //przypisujemy testowaną flagę
    sigemptyset(&sa.sa_mask);

    sigaction(SIGUSR1, &sa, NULL); //wskazujemy obslugiwanie sygnallow
    printf("SIGINFO TEST\n");
    raise(SIGUSR1); //wysylamy sygnal
    return 0;
}