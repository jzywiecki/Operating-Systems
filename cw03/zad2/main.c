#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char * argv[]){
    if (argc < 2){
        printf("Not enaugh arguments!\n");
        exit(1);
    }
    if (argc > 2){
        printf("Too many arguments!\n");
        exit(1);
    }

    setbuf(stdout, NULL); // wyłączenie buforowania wyjścia
    printf("Program name: %s ", argv[0]); //wypisanie nazwy programu bez znaku konca linii
    execl("/bin/ls", "ls", "-l", argv[1], NULL); //wywolanie bin/ls ls z argumentem -l (czytelniej) i katalogiem

    //to wykona sie tylko w przypadku, kiedy wystapi blad!
    printf("An error occured executing execl!");
    exit(1);
}

