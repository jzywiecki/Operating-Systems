#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>



void handler_siginfo(int signum, siginfo_t *info, void *context){
    if(signum == SIGUSR1){
        printf("SIGNAL SIGUSR1, PID: %d, SIGNAL: %d\n", info->si_pid, signum);
    }
    else if (signum == SIGUSR2){
        printf("SIGNAL SIGUSR2, PID: %d, SIGNAL: %d\n", info->si_pid, signum);
    }
    else{
        printf("SIGNAL NOT SUPPORTED, PID: %d, SIGNAL: %d\n", info->si_pid, signum);
    }
}

void handler_resethand(int signum, siginfo_t *info, void *context){

}

void handler_nodefer(int signum, siginfo_t *info, void *context){
    if(signum == SIGHUP){
        printf("SIGNAL SIGHUP, PID: %d, SIGNAL: %d\n", info->si_pid, signum);
    }
}



void test_siginfo(){
    struct sigaction act;
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);

    if (sigaction(SIGUSR1, &act, NULL) == -1){
        perror("Error sigaction SIGUSR1!\n");
        exit(1);
    }
    if (sigaction(SIGUSR2, &act, NULL) == -1){
        perror("Error sigaction SIGUSR2!\n");
        exit(1);
    }
    if (sigaction(SIGINT, &act, NULL) == -1){
        perror("Error sigaction SIGRTMIN!\n");
        exit(1);
    }

    printf("\nSIGINFO TEST\n");
    raise(SIGUSR1);
    raise(SIGUSR2);
    raise(SIGINT);
}


void test_nodefer(){
    struct sigaction sa;
    sa.sa_flags = SA_NODEFER | SA_SIGINFO;
    sa.sa_sigaction = handler_resethand;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGHUP, &sa, NULL);

    printf("Program oczekuje na sygnał SIGHUP...\n");

    for (int i = 0; i < 3; i++) {
        sleep(1);
        raise(SIGHUP);
    }

    return 0;
}


void test_resethand(){

}


int main(){
    test_siginfo();
    test_nocldstop();
    test_resethand();
    return 0;
}