#include <stdio.h>
#include <sys/fcntl.h>
#include "timers.h"


clock_t clock_start;
clock_t clock_end;
struct tms start_tms;
struct tms end_tms;

void switch_letters_lib(char from, char to, char* file_from_name, char* file_to_name){
    FILE *file_from = fopen(file_from_name, "r");
    FILE * file_to = fopen(file_to_name, "w");

    if (file_from == NULL){ //simple error handling
        printf("Could not open input file!\n");
        return;
    }
    if (file_to == NULL){ //if we dont close file_from there we potentially will have memory leaks, file would remain open etc.
        printf("Could not open output file!\n");
        fclose(file_from);
        return;
    }

    char c;
    while (fread(&c, sizeof(char), 1, file_from)){ // till end of file
        if (c == from){
            fwrite(&to, 1, 1, file_to); //write "to" if it letter is equal "from"
        }
        else{
            fwrite(&c, sizeof(char), 1, file_to); //just write letter
        }
    }
    fclose(file_to);
    fclose(file_from);
}

void switch_letters_sys(char from, char to, char* file_from_name, char* file_to_name){
    char c;
    int file_from = open(file_from_name, O_RDONLY);
    int file_to = open(file_to_name, O_WRONLY|O_CREAT, S_IRUSR | S_IWUSR); //O_WRONLY - pisanie, O_CREAT - bedzie stworzony jak nie istnieje, S_IRUSR - uprawinienia czytania, S_IWUSR - uprawnienia pisania

    if (file_from == -1){ //simple error handling
        printf("Could not open input file!\n");
        return;
    }
    if (file_to == -1){ //if we dont close file_from there we potentially will have memory leaks, file would remain open etc.
        printf("Could not open output file!\n");
        close(file_from);
        return;
    }

    while(read(file_from,&c,1) > 0) { //in this case read function should return 1, but we can write >0
        if (c == from) {
            write(file_to, &to, 1); //write "to" if it letter is equal "from"
        }
        else{
            write(file_to, &c, 1); //just write letter
        }
    }
}

int main(int argc, char* argv[]){
    if (argc != 5){
        printf("Not enough or too much arguments!\n");
    }
    printf("Library:\n");
    clock_start = times(&start_tms);
    switch_letters_lib(argv[1][0], argv[2][0], argv[3], argv[4]);
    clock_end = times(&end_tms);
    print_results(clock_start, clock_end, start_tms, end_tms);

    printf("System:\n");
    clock_start = times(&start_tms);
    switch_letters_sys(argv[1][0], argv[2][0], argv[3], argv[4]);
    clock_end = times(&end_tms);
    print_results(clock_start, clock_end, start_tms, end_tms);
}

