#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>


void handler(int signum){ //handler
    printf("PID: %d, Recieved signal: %d\n", getpid(), signum);
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

    sigset_t set; //utworzenie zbiorów sygnałów
    sigemptyset(&set); //inicjalizacja pustego zbioru
    sigaddset(&set, SIGUSR1); //dodanie sygnału SIGUSR1

    if (strcmp(argv[1], "ignore") == 0) { //ignore
        signal(SIGUSR1, SIG_IGN); //SIG_IGN ustawia ignorowanie
        printf("SIGUSR1 ignored in parent\n");
    } else if (strcmp(argv[1], "handler") == 0) { //podpięcie handlera
        signal(SIGUSR1, handler); //instalujemy handler dla SIGUSR1
        printf("SIGUSR1 handled in parent\n");
    } else if (strcmp(argv[1], "mask") == 0) { //maskowanie (blokowanie) sygnału
        sigprocmask(SIG_BLOCK, &set, NULL); //dodajemy nowe sygnały do blokowania do maski
        printf("SIGUSR1 masked in parent\n");
    } else if (strcmp(argv[1], "pending") == 0) {
        sigprocmask(SIG_BLOCK, &set, NULL); //dodajemy nowe sygnały do blokowania do maski
        printf("SIGUSR1 masked (for pending) in parent\n");
    } else {
        printf("Invalid argument: %s\n", argv[1]); //obsługa błędnego argumentu
        exit(1);
    }

    raise(SIGUSR1); //wysłanie sygnału z procesu do samego siebie

    pid_t pid = fork(); //fork

     if (pid == 0) { //dla dziecka
        if (strcmp(argv[1], "pending") == 0) {
            sigpending(&set); //odczytanie zbioru blokowanych sygnałów do set
            if (sigismember(&set, SIGUSR1)) { //jesli proces w zbiorze tzn. ze ma status pending
                printf("[FORK] SIGUSR1 is pending in child\n");
            }
            else{
                printf("[FORK] SIGUSR1 is not pending in child!\n");
            }
        }
        else{
            raise(SIGUSR1); //wysłanie sygnału dziecka do samego siebie
        }
        exit(0); //wyjscie dzieckiem, aby móc kontrolowanie wywołać execl używając parenta
    }

     if (strcmp(argv[1], "handler") != 0) {
         execl("./exec", "./exec", argv[1], NULL); //test dla exec, warto zwrocic uwage, ze nadpisujemy proces, wiec bedzie on mial to samo pid
     }
     return 0;
}
