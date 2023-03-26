#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int signum) {
    printf("Received signal %d\n", signum);
}

void run_ignore() {
    printf("Setting signal to be ignored\n");
    signal(SIGUSR1, SIG_IGN);
    raise(SIGUSR1);
}

void run_handler() {
    printf("Setting signal handler\n");
    signal(SIGUSR1, sig_handler);
    raise(SIGUSR1);
}

void run_mask() {
    printf("Masking signal\n");
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, NULL);
    raise(SIGUSR1);
    int pending = sigpending(&set);
    if (pending == 0) {
        printf("Signal is pending\n");
    } else {
        printf("Signal is not pending\n");
    }
}

void run_pending() {
    printf("Testing pending signal in parent\n");
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    raise(SIGUSR1);
    int pending = sigpending(&set);
    if (pending == 0) {
        printf("Signal is pending in parent\n");
    } else {
        printf("Signal is not pending in parent\n");
    }
    pid_t pid = fork();
    if (pid == 0) {
        printf("Testing pending signal in child\n");
        sigset_t child_set;
        sigpending(&child_set);
        if (sigismember(&child_set, SIGUSR1)) {
            printf("Signal is pending in child\n");
        } else {
            printf("Signal is not pending in child\n");
        }
        exit(0);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        printf("Error creating child process\n");
    }
}