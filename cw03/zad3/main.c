#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

void recursive_call_directory(char* dir_path, char* string_to_find){
    DIR *dir = opendir(dir_path);
    if (dir == NULL){
        printf("Error entering directory!\n");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL){
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) { //unikamy w ten sposob wchodzenia glebiej
            continue;
        }

        char path[255];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name); //konstrukcja sciezki do pliku/nastepnego folderu

        struct stat statbuf;
        if (stat(path, &statbuf) == -1) {
            printf("Error getting stat of path!\n");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)){ //katalog
            pid_t pid = fork(); //forkujemy
            if (pid == -1){
                printf("Error while forking!\n");
                continue;
            }

            if (pid == 0){ //dla nowo utworzonego procesu wchodzimy do tego katalogu
                recursive_call_directory(path, string_to_find);
                exit(EXIT_SUCCESS);
            }
        }
        else if (S_ISREG(statbuf.st_mode)) { //plik
            FILE *file = fopen(path, "r");
            if (file == NULL) {
                printf("Could not open file!\n");
                continue;
            }

            size_t line_len = strlen(string_to_find); //dlugosc sprawdzanego fragmentu
            char *line = calloc(line_len + 1, sizeof(char)); //bufor do odczytania z pliku

            if (fread(line, sizeof(char), line_len, file) != 0) {
                if (strncmp(line, string_to_find) == 0) {
                    printf("%s, %d\n", path, getppid()); //jesli fragmenty sa takie same wypisujemy sciezkie oraz pid
                    break;
                }
            }
            else {
                continue;
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



