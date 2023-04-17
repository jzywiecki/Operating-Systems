#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "timers.h"


double integral(double a, double b, double precision) { //funkcja odpowiedzialna za całkowanie
    double sum = 0.0; //calkowita suma
    for (double i = a; i < b; i += precision) //przechodzimy od a do b i liczymy tę sumę dla zadanej precyzji, tzn. to szerokosc prostokąta
        sum += (4.0 / (i * i + 1.0)) * precision;
    return sum;
}

clock_t clock_start; //do czasów
clock_t clock_end;
struct tms start_tms;
struct tms end_tms;

int main(int argc, char* argv[]){
    if (argc < 3){ //obsluga blednych argumentów
        printf("Not enaugh arguments!\n");
        exit(1);
    }
    if (argc > 3){
        printf("Too many arguments!\n");
        exit(1);
    }
    clock_start = times(&start_tms); //wlaczamy zegar, zaczynamy liczyc czas

    double precision = atof(argv[1]);
    int number_of_children = atoi(argv[2]);

    int pipes[number_of_children]; //file descriptory odpowiedzialne za czytanie rodziców
    pid_t pid[number_of_children]; //pid dzieci

    for (int i = 0; i < number_of_children; i++){ //dzielimy na procesy wykonujące obliczenia
        int fd[2]; //dla potoku
        pipe(fd); //potok
        if (fork() == 0){ //dziecko
            pid[i] = getpid(); //zapisujemy pid procesu dziecka
            close(fd[0]); //zamykamy czytanie dla dziecka
            double a = (double) i / number_of_children;
            double b = (double) (i + 1) / number_of_children;
            double result = integral(a, b, precision); //liczymy te calke
            write(fd[1], &result, sizeof(result)); //piszemy do potoku
            close(fd[1]); //zamykamy pisanie dla dziecka
            exit(0); //konczymy proces dziecka
        }
        else{
            pipes[i] = fd[0]; //zapisujemy file descriptor odpowiedzialny za czytanie dla rodzica
            close(fd[1]); //zamykamy proces pisania dla rodzica
        }
    }

    double result = 0.0; //wynik
    for (int i = 0; i < number_of_children; i++) { //
        int status;
        waitpid(pid[i], &status, 0);

        double subresult; //podwynik
        read(pipes[i], &subresult, sizeof(subresult)); //czytamy podwynik
        result += subresult; //dodajemy podwynik do wyniku końcowego
        close(pipes[i]); //konczymy czytanie z danego strumienia
    }

    clock_end = times(&end_tms); //konczymy zegar
    printf("Integral value: %f\n", result); //wypisujemy wynik
    print_results(clock_start, clock_end, start_tms, end_tms); //wypisujemy czasy


    return 0;
}