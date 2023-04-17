#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

double integral(double a, double b, double precision) { //funkcja odpowiedzialna za całkowanie
    double sum = 0.0; //calkowita suma
    for (double i = a; i < b; i += precision) //przechodzimy od a do b i liczymy tę sumę dla zadanej precyzji, tzn. to szerokosc prostokąta
        sum += (4.0 / (i * i + 1.0)) * precision;
    return sum;
}

int main(int argc, char* argv[]){
//    setbuf(stdout, NULL);
    double a = atof(argv[1]);
    double b = atof(argv[2]);
    double precision = atof(argv[3]);

    double result = integral(a, b, precision);
//    printf("%f", result);

    int pipe = open("integralpipe", O_WRONLY); //otworzenie potoku w trybie pisania
    write(pipe, &result, sizeof(result)); //zapisanie do potoku
    close(pipe); //zamkniecie potoku
    return 0;
}