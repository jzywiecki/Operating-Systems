#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>



void handler(int signum){
    printf("Recieved signal: %d", signum);
}

int main(int argc, char* argv[]){
    if (argc < 2) { //obsluga wprowadzania argumentow
        printf("Not enough arguments!\n");
        exit(1);
    }
    if (argc > 2){
        printf("Too many arguments!\n");
        exit(1);
    }

    sigset_t set, old_set; //utworzenie zbiorów sygnałów
    sigemptyset(&set); //inicjalizacja pustego zbioru
    sigaddset(&set, SIGUSR1); //dodanie sygnału SIGUSR1

    if (strcmp(argv[1], "ignore") == 0) { //ignore
        signal(SIGUSR1, SIG_IGN); //SIG_IGN ustawia ignorowanie
        printf("SIGUSR1 ignored in parent\n");
    } else if (strcmp(argv[1], "handler") == 0) { //podpięcie handlera
        signal(SIGUSR1, handler); //instalujemy handler dla SIGUSR1
        printf("SIGUSR1 handled in parent\n");
    } else if (strcmp(argv[1], "mask") == 0) { //maskowanie (blokowanie) sygnału
        sigprocmask(SIG_BLOCK, &set, &old_set); //dodajemy nowe sygnały do blokowania do maski, stara maske zapisujemy w old_set
        printf("SIGUSR1 masked in parent\n");
    } else if (strcmp(argv[1], "pending") == 0) {
        sigprocmask(SIG_BLOCK, &set, &old_set); //dodajemy nowe sygnały do blokowania do maski, stara maske zapisujemy w old_set
        printf("SIGUSR1 masked in parent\n");
        if (sigismember(&old_set, SIGUSR1)) { //jeśli był wcześniej zablokowany
            printf("SIGUSR1 is pending in parent\n");
        }
    } else {
        printf("Invalid argument: %s\n", argv[1]); //obsługa błędnego argumentu
        exit(1);
    }

    raise(SIGUSR1); //wysłanie sygnału z procesu do samego siebie

    pid_t pid = fork(); //fork

     if (pid == 0) { //dziecko
        printf("Child process created\n");
        raise(SIGUSR1); //wysłanie sygnału dziecka do samego siebie
        if (strcmp(argv[1], "pending") == 0) {
            if (sigismember(&old_set, SIGUSR1)) { //jeśli był wcześniej zablokowany
                printf("SIGUSR1 is pending in child\n");
            }
        }
        exit(0);
    } else { //rodzic
        printf("Parent process exiting\n");
        exit(0);
    }



}
