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
        perror("opendir"); //obsluga bledu
        exit(1);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL){ //do czasu az mamy elementy w katalogu
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) { //unikamy w ten sposob wchodzenia glebiej, zgodnie ze wskazówką
            continue;
        }

        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name); //konstrukcja sciezki do pliku lub katalogu

        struct stat path_stat; //potrzebny do sprawdzania informacji na temat pliku/katalogu
        stat(path, &path_stat);

        if (S_ISDIR(path_stat.st_mode)){ //katalog
            pid_t pid = fork(); //fork
            if (pid == 0){ //dla nowo utworzonego procesu wchodzimy do tego katalogu rekursywnie, w ten sposob uzyskamy drzewo katalogow i procesow
                recursive_call_directory(path, string_to_find);
                exit(0);
            }
        }
        else if (S_ISREG(path_stat.st_mode)){ //plik
            FILE *file = fopen(path, "r");
            if (file == NULL){ //obsluga bledow przy otwarciu pliku
                perror("fopen"); //obsluga bledu
                continue; //nie musimy konczyc procesu, bo uwzgledniamy, ze niektorych plikow nie mozemy otworzyc
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
        perror("closedir"); //obsluga bledu
        exit(1);
    }
}

int main(int argc, char* argv[]){
    if (argc < 3){
        printf("Not enaugh arguments!\n");
        exit(1);
    }
    if (argc > 3){
        printf("Too many arguments!\n");
        exit(1);
    }

    recursive_call_directory(argv[1], argv[2]);
    return 0;
}



