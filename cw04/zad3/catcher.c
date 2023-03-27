#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>

int mode_changes = 0;
volatile sig_atomic_t mode = 0;

void handler(int signum, siginfo_t *info, void *context) {
    setbuf(stdout, NULL); //ustawiam bufor na NULL dla bezpieczenstwa
    printf("Signal received by catcher!\n");
    kill(info->si_pid, SIGUSR2); //wyslanie SIGUSR2 do sendera, potwierdzenie otrzymania

    int new_mode = info->si_value.sival_int; //wyciagniecie jaki tryb pracy mamy przyjąć
    printf("Mode received: %d\n", new_mode);

    switch (new_mode) { //ustawienie trybu pracy
        case 1:
            mode_changes++;
            for (int i = 1; i <= 100; i++){ //wypisywanie cyfr
                printf("%d ", i);
            }
            printf("\n");
            break;
        case 2: {
            mode_changes++;
            time_t current_time = time(NULL); //czas
            printf("%s", ctime(&current_time));
            break;
        }
        case 3:
            mode_changes++;
            printf("Number of mode changes received: %d\n", mode_changes); //ilosc zmian trybu pracy
            break;
        case 4:
            mode_changes++; //przypadek nieskonczonej petli wypisujacej czas co sekunde rozwiazujemy to troche nizej w kodzie
            break;
        case 5:
            mode_changes++;
            exit(0); //zakonczenie pracy
            break;
        default:
            printf("Invalid mode: %d\n", new_mode); //obsluga bledow
            break;
    }

    union sigval value;
    value.sival_int = new_mode;
    sigqueue(info->si_pid, SIGUSR1, value); //wyslanie sygnalu SIGUSR1 do sendera potwierdzajace zakonczenie wykonywania

    mode = new_mode;
}

int main() {
    struct sigaction action;
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = handler;
    sigaction(SIGUSR1, &action, NULL); //ustawienie handlera dla SIGUSR1

    printf("My PID is %d\n", getpid()); //wypisanie PID catchera

    while (1) {
        while (mode == 4) { //obsluga trybu nieskonczonej petli
            time_t current_time = time(NULL);
            printf("%s", ctime(&current_time));
            sleep(1);
        }
    }

    return 0;
}