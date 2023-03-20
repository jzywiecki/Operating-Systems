#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char * argv[]){
    if (argc < 2){
        printf("Not enaugh arguments!\n");
        exit(EXIT_FAILURE);
    }
    if (argc > 2){
        printf("Too many arguments!\n");
        exit(EXIT_FAILURE);
    }

    int number_of_processes = atoi(argv[1]); //odczytanie ilosci procesow

    if (number_of_processes <= 0){ // atoi returns 0 if an error occured, so it is handling it as well
        printf("Bad number of arguments!\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < number_of_processes; i++){
        pid_t pid = fork();
        if (pid == 0){ //dla dziecka
            printf("Parent: %d, child: %d\n", getppid(), getpid()); //wypisanie dziecka pid i parentpid
            exit(EXIT_SUCCESS); //prawidlowe zakonczenie procesu
        }
        else{
            waitpid(pid, NULL, 0); //proces macierzysty czeka na dziecko (pid dziecka jest zapisane pod zmienną pid)
        }
    }

    printf("%s\n", argv[1]); //wypisanie ilosci procesow lacznie
    return 0;
}

