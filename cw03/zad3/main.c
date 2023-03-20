#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

void recursive_call_directory(char* dir_path, char* string_to_find){
    DIR *dir = opendir(dir_path);
    if (dir == NULL){ //katalog nie istnieje
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL){ //do czasu az mamy elementy w katalogu
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) { //unikamy w ten sposob wchodzenia glebiej, zgodnie ze wskazówką
            continue;
        }

        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name); //konstrukcja sciezki do pliku lub nastepnego folderu

        struct stat statbuf; //potrzebny do sprawdzania informacji na temat pliku/katalogu
        if (stat(path, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)){ //katalog
            pid_t pid = fork(); //fork
            if (pid == -1){ //obsluga bledu przy forkowaniu
                perror("fork");
                continue;
            }

            if (pid == 0){ //dla nowo utworzonego procesu wchodzimy do tego katalogu rekursywnie, w ten sposob uzyskamy drzewo katalogow i procesow
                recursive_call_directory(path, string_to_find);
                exit(EXIT_SUCCESS);
            }
        }
        else if (S_ISREG(statbuf.st_mode)){ //plik
            FILE *file = fopen(path, "r");
            if (file == NULL){ //obsluga bledow przy otwarciu pliku
                perror("fopen");
                continue;
            }

            size_t line_len = strlen(string_to_find); //dlugosc sprawdzanego fragmentu
            char *line = calloc(line_len, sizeof(char)); //bufor do odczytania znaków z pliku

            if (fread(line, sizeof(char), line_len, file) != 0) { //czytamy linię z pliku, co wazne w przypadku gdy plik bedzie za krotki tu tez to obslugujemy
                if (strcmp(line, string_to_find) == 0) {
                    printf("%s, %d\n", path, getppid()); //jesli fragmenty sa takie same wypisujemy sciezkie oraz pid
                }
            }
            //zwolnienie zasobow i zamkniecie plikow
            free(line);
            fclose(file);
        }

    }

    if (closedir(dir) == -1) {
        perror("closedir");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]){
    if (argc < 3){
        printf("Not enaugh arguments!\n");
        exit(EXIT_FAILURE);
    }
    if (argc > 3){
        printf("Too many arguments!\n");
        exit(EXIT_FAILURE);
    }

    recursive_call_directory(argv[1], argv[2]);
    return 0;
}



