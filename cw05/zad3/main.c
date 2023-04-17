#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "timers.h"

clock_t clock_start; //do czasów
clock_t clock_end;
struct tms start_tms;
struct tms end_tms;

int main(int argc, char* argv[]) {
    if (argc < 3){ //obsluga blednych argumentów
        printf("Not enaugh arguments!\n");
        exit(1);
    }
    if (argc > 3){
        printf("Too many arguments!\n");
        exit(1);
    }
    clock_start = times(&start_tms); //wlaczamy zegar, zaczynamy liczyc czas

    int number_of_programs = atoi(argv[2]);
    char a[100]; //tablice na liczby wejsciowe do podprogramów
    char b[100];

    mkfifo("integralpipe", 0666);

    for (int i = 0; i < number_of_programs; i++) {
        if (fork() == 0) { //dziecko
            snprintf(a, sizeof(a), "%lf", (double) i / number_of_programs); //zapisujemy pod a dolny kraniec przedzialu
            snprintf(b, sizeof(b), "%lf", (double) (i + 1) / number_of_programs); //zapisujemy pod b gorny kraniec przedzialu
            execl("./integral", "integral", a, b, argv[1], NULL); //nadpisujemy proces uruchamiajac nasz integral, w ktorym liczymy calke
            printf("Blad!\n"); //blad podprogramu
            exit(1);
        }
    }


    double result = 0.0; //wynik
    int pipe = open("integralpipe", O_RDONLY); //otwieramy potok do czytania

    for (int i = 0; i < number_of_programs; i++) { //oczekujemy az kazdy proces zakonczy prace
        wait(NULL);
    }

    double subresult;
    for (int i = 0; i < number_of_programs; i++) {
        if (read(pipe, &subresult, sizeof(subresult)) == -1) { //obsluga bledu i czytanie z potoku podwyniku
            perror("read");
            return 1;
        }
        result += subresult; //dodajemy podwyniki
    }

    close(pipe); //zamykamy potok
    remove("integralpipe"); //usuwamy potok

    clock_end = times(&end_tms); //konczymy mierzenie czasu
    printf("Integral value: %f\n", result); //wypisujemy wynik
    print_results(clock_start, clock_end, start_tms, end_tms); //wypisujemy czasy

    return 0;
}
