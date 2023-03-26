#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

volatile sig_atomic_t received_ack = 0;
volatile sig_atomic_t requests = 0;
volatile sig_atomic_t mode = 0;

void signal_handler(int sig, siginfo_t *info, void *context) {
    if (sig == SIGUSR1) {
        printf("Sender: Received SIGUSR1 from catcher!\n");
        received_ack = 1;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Not enough arguments!\n");
        exit(1);
    }
    pid_t catcher_pid = atoi(argv[1]);
    int modes_to_send[argc-2];

    for (int i = 0; i < argc-2; i++) {
        modes_to_send[i] = atoi(argv[i+2]);
    }

    struct sigaction sa;
    sigset_t mask;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    for (int i = 0; i < argc - 2; i++) {
        mode = modes_to_send[i];
        requests++;
        printf("Sender: Sending SIGUSR1 to catcher with PID %d, mode %d\n", catcher_pid, mode);

        if (sigqueue(catcher_pid, SIGUSR1, (union sigval) mode) == -1) {
            perror("sigqueue");
            exit(1);
        }

        while (!received_ack) {
            sigemptyset(&mask);
            sigaddset(&mask, SIGUSR1);
            sigsuspend(&mask);
        }

        received_ack = 0;
    }

    printf("Sender: All requests sent and acknowledged. Exiting.\n");
    return 0;
}