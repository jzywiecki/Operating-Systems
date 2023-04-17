#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Zgodnie z ustaleniami z zajęć prezentuje rozwiązanie
 * przy pomocy funkcji cowsay oraz fortune. Aby rozróżnić argumenty
 * dla 1 argumentu będzie to "fortune", a dla 3 zdanie użytkownika powtórzone x razy.
 * Zatem użycia:
 *
 * ./main fortune
 * ./main mine message message_repetition
 * message_repetiton wieksze od 0
 *
 * */


int main(int argc, char* argv[]){
    char message[200] = ""; //dlugosc fortune jest domyslnie ustawiona na 160 znaków, dajemy zapas
    if (argc == 2){ // dla 1 argumentu
        if (strcmp(argv[1], "fortune") == 0){
            FILE* fortune = popen("fortune", "r");
            size_t size = fread(message, sizeof(char), 200, fortune); //zapisanie wiadomosci z fortune do message
            message[size] = '\0'; //zakonczenie wiadomosci
            pclose(fortune);

            FILE * cowsay = popen("cowsay", "w");
            fwrite(message, sizeof(char), strlen(message), cowsay); //wpisanie wiadomosci z message do cowsay
            pclose(cowsay);
            exit(0);
        }
        else{
            printf("Not proper usage!\n");
            exit(1);
        }
    }
    if (argc == 4){ //dla 3 argumentów
        if (strcmp(argv[1], "mine") == 0){
            int repetitions = atoi(argv[3]);
            if (repetitions == 0){
                printf("Not proper usage!\n");
                exit(1);
            }
            strcpy(message, argv[2]); //najpierw kopiujemy wiadomosc, potem doklejamy do niej nastepne powtorzenia
            for (int i = 0; i < repetitions-1; i++){ //zapisanie powtorzonej wiadomosci do message
                strcat(message, argv[2]);
            }

            message[strlen(message)] = '\0'; //zakonczenie wiadomosci

            FILE * cowsay = popen("cowsay", "w");
            fwrite(message, sizeof(char), strlen(message), cowsay); //wpisanie wiadomosci z message do cowsay
            pclose(cowsay);
            exit(0);
        }
        else{
            printf("Not proper usage!\n");
            exit(1);
        }
    }
    printf("Not proper usage!");
    exit(1);
}