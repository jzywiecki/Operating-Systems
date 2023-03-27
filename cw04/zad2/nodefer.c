#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum, siginfo_t *info, void *context){
    if (signum == SIGINT){
        printf("SIGNAL SIGINT, PID: %d, SIGNAL: %d\n", info->si_pid, signum);
    }
    else{
        printf("SIGNAL NOT SUPPORTED, PID: %d, SIGNAL: %d\n", info->si_pid, signum);
    }
}

int main() {
    struct sigaction sa;
    sa.sa_sigaction = handler; //instalujemy handler
    sigemptyset(&sa.sa_mask); //pusta maska
    sa.sa_flags = SA_SIGINFO | SA_NODEFER; //przypisujemy testowaną flagę

    sigaction(SIGINT, &sa, NULL);//wskazujemy obslugiwanie SIGINT'a

    printf("NODEFER TEST\n");
    printf("Sending SIGINT signal to the process...\n");
    raise(SIGINT); //pierwszy sygnal


    printf("Sending another SIGINT signal to the process...\n");
    raise(SIGINT); //drugi powinien sie wyswietlic
    return 0;
}