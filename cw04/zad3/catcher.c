#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>


int mode_changes = 0;
volatile sig_atomic_t stop_mode_four = 0;

void handler(int signum, siginfo_t *info, void *context){
    printf("Signal SIGUSR1 received by catcher\n");
    kill(info->si_pid, SIGUSR2);

    int mode = info->si_value.sival_int;
    printf("Mode received: %d\n", mode);


    switch (mode) {
        case 1:
            for (int i = 1; i <= 100; i++){
                printf("%d ", i);
            }
            printf("\n");
            break;
        case 2:
            time_t current_time = time(NULL);
            printf("%s", ctime(&current_time));
            break;
        case 3:
            mode_changes++;
            printf("Number of mode changes received: %d\n", mode_changes);
            break;
        case 4:
            while (mode == 4) {
                time_t t = time(NULL);
                printf("%s", ctime(&t));
                sleep(1);
                if (stop_mode_four) {
                    stop_mode_four = 0;
                    break;
                }
            }
            break;
        case 5:
            exit(1);
            break;
        default:
            printf("Invalid mode: %d\n", mode);
            break;
    }
    union sigval value;
    value.sival_int = mode;
    if (sigqueue(info->si_pid, SIGUSR1, value) == -1) {
        perror("Catcher: Error sending signal to sender");
        exit(1);
    }

}


int main(){
    struct sigaction action;
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = handler;
    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("Catcher: Error setting signal handler");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGUSR2, SIG_IGN) == SIG_ERR) {
        perror("Catcher: Error setting signal handler");
        exit(EXIT_FAILURE);
    }

    printf("Catcher: My PID is %d\n", getpid());
    printf("Catcher: Waiting for signals...\n");

    while (1) {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR2);
        sigprocmask(SIG_SETMASK, &mask, NULL);
        sigsuspend(&mask);
    }

    return 0;

}