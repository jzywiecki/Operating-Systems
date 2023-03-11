#include <stdio.h>
#include <sys/fcntl.h>
#include "timers.h"


clock_t clock_start;
clock_t clock_end;
struct tms start_tms;
struct tms end_tms;

void reverse_file_bytes(char* file_from_name, char* file_to_name){
    FILE *file_from = fopen(file_from_name, "r");
    FILE *file_to = fopen(file_to_name, "w");

    if (file_from == NULL || file_to == NULL){
        printf("Could not open one of the files!\n");
        return;
    }
    char c;
    int size = 0;
    while(fread(&c, sizeof(char), 1, file_from)){
        size += 1;
    }
    rewind(file_from);
    for (int i = 1; i <= size; i++){
        fseek(file_from, -i, 2);
        fread(&c, sizeof(char), 1, file_from);
        fwrite(&c, sizeof(char), 1, file_to);
    }

    fclose(file_from);
    fclose(file_to);
}


void reverse_file_blocks(char* file_from_name, char* file_to_name){
    FILE *file_from = fopen(file_from_name, "r");
    FILE *file_to = fopen(file_to_name, "w");



    fclose(file_from);
    fclose(file_to);
}

int main(){
    clock_start = times(&start_tms);
    reverse_file_bytes("file_from.txt", "file_to.txt");
    clock_end = times(&end_tms);
    print_results(clock_start, clock_end, start_tms, end_tms);
    clock_start = times(&start_tms);
//    reverse_file_blocks();
    clock_end = times(&end_tms);
    print_results(clock_start, clock_end, start_tms, end_tms);
}

