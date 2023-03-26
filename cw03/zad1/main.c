#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char * argv[]){
    if (argc < 2){
        printf("Not enaugh arguments!\n");
        exit(1);
    }
    if (argc > 2){
        printf("Too many arguments!\n");
        exit(1);
    }

    int number_of_processes = atoi(argv[1]); //odczytanie ilosci procesow

    if (number_of_processes <= 0){ // atoi zwraca 0 kiedy wystapi blad, wiec to rowniez obslugujemy
        printf("Bad number of arguments or argument is illegal!\n");
        exit(1);
    }

    for (int i = 0; i < number_of_processes; i++){
        pid_t pid = fork();
        if (pid == 0){ //dla dziecka
            printf("Parent: %d, child: %d\n", getppid(), getpid()); //wypisanie dziecka pid i parentpid
            exit(0); //prawidlowe zakonczenie procesu
        }
        else{
            waitpid(pid, NULL, 0); //proces macierzysty czeka na dziecko (pid dziecka jest zapisane pod zmienną pid)
        }
    }

    printf("%s\n", argv[1]); //wypisanie ilosci procesow lacznie
    return 0;
}

