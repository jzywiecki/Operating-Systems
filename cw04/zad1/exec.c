#include <stdio.h>
#include <signal.h>
#include <string.h>


int main(int argc, char* argv[]) {
    if (strcmp(argv[1], "pending") == 0) {
        sigset_t set; //tworzymy zbiór
        sigpending(&set); //odczytanie zbioru blokowanych sygnałów do set
        if (sigismember(&set, SIGUSR1)) { //jeśli nalezy do zbioru
            printf("[EXEC] SIGUSR1 is pending in child\n");
        } else {
            printf("[EXEC] SIGUSR1 is not pending in child!\n");
        }
    }
    else{
        raise(SIGUSR1); //puszczamy sygnał w przypadku gdy nie jest pending
    }
    return 0;
}
