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

    char c;
    while (fread(&c, sizeof(char), 1, file_from)){
        if (c == from){
            fwrite(&to, 1, 1, file_to);
        }
        else{
            fwrite(&c, sizeof(char), 1, file_to);
        }
    }
    fclose(file_to);
    fclose(file_from);
}

void switch_letters_sys(char from, char to, char* file_from_name, char* file_to_name){
    char c;
    int file_from, file_to;
    file_from = open(file_from_name, O_RDONLY);
    file_to = open(file_to_name, O_WRONLY|O_CREAT, S_IRUSR | S_IWUSR); //O_WRONLY - pisanie, O_CREAT - bedzie stworzony jak nie istnieje, S_IRUSR - uprawinienia czytania, S_IWUSR - uprawnienia pisania
    while(read(file_from,&c,1)==1) {
        if (c == from) {
            write(file_to, &to, 1);
        }
        else{
            write(file_to, &c, 1);
        }
    }
}

int main(){
    char a = 'a';
    char c = 'c';
    clock_start = times(&start_tms);
    switch_letters_lib(a, c, "file_from.txt", "file_to.txt");
    clock_end = times(&end_tms);
    print_results(clock_start, clock_end, start_tms, end_tms);
    clock_start = times(&start_tms);
    switch_letters_sys(c, a, "file_from.txt", "file_to.txt");
    clock_end = times(&end_tms);
    print_results(clock_start, clock_end, start_tms, end_tms);
}

