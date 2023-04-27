#include <stdio.h>


int main(int argc, char* argv[]){
    setbuf(stdout, NULL);

    //podpinamy pamiec wspolna
    char* shared_memory;

    //otwieramy semafory

    //musimy w kolko obslugiwac klientów
    while (true){

    }

    printf("Finished work!\n");

    //odpinamy pamiec wspolna
    shmdt(shared_memory);

    return 0;
}







