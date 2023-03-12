#include <stdio.h>
#include "timers.h"
#include <string.h>


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
    fseek(file_from, 0, SEEK_END);
    long size = ftell(file_from);
    fseek(file_from, 0, SEEK_SET);

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
    FILE *file_from = fopen(file_from_name, "rb");
    FILE *file_to = fopen(file_to_name, "wb");

    if (file_from == NULL || file_to == NULL){
        printf("Could not open one of the files!\n");
        return;
    }
    char block_content[1024];
    size_t bytes_read;

    while ((bytes_read = fread(block_content, 1, sizeof(block_content), file_from)) > 0){
        for (int i = 0; i < bytes_read / 2; i++){
            char temp = block_content[i];
            block_content[i] = block_content[bytes_read - 1 - i];
            block_content[bytes_read - 1 - i] = temp;
        }
        fwrite(block_content, 1, bytes_read, file_to);
    }

    fclose(file_from);
    fclose(file_to);
}

int main(){
    clock_start = times(&start_tms);
    reverse_file_bytes("file_from.txt", "file_to.txt");
    clock_end = times(&end_tms);
    print_results(clock_start, clock_end, start_tms, end_tms);
    clock_start = times(&start_tms);
    reverse_file_blocks("file_from.txt", "file_to.txt");
    clock_end = times(&end_tms);
    print_results(clock_start, clock_end, start_tms, end_tms);
}

