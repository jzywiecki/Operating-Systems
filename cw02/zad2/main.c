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

    if (file_from == NULL){ //simple error handling
        printf("Could not open input file!\n");
        return;
    }
    if (file_to == NULL){ //if we dont close file_from there we potentially will have resources leaks, file would remain open etc.
        printf("Could not open output file!\n");
        fclose(file_from);
        return;
    }

    char c;
    fseek(file_from, 0, SEEK_END); //go to the end of file to check length of file
    long size = ftell(file_from);

    for (int i = 1; i <= size; i++){ //go through file. Because we are counting from the end (SEEK_END), we have to go from 1 to <= size
        fseek(file_from, -i, SEEK_END); //move cursor
        fread(&c, sizeof(char), 1, file_from);
        fwrite(&c, sizeof(char), 1, file_to);
    }
    fclose(file_from);
    fclose(file_to);
}


void reverse_file_blocks(char* file_from_name, char* file_to_name){
    FILE *file_from = fopen(file_from_name, "r");
    FILE *file_to = fopen(file_to_name, "w");

    if (file_from == NULL){ //simple error handling
        printf("Could not open input file!\n");
        return;
    }
    if (file_to == NULL){ //if we dont close file_from there we potentially will have resources leaks, file would remain open etc.
        printf("Could not open output file!\n");
        fclose(file_from);
        return;
    }

    char block_content[1024]; //block to save our content
    size_t bytes_read; //actual size of red blocks

    fseek(file_from, 0, SEEK_END); //go to the end of file to check length of file
    long size = ftell(file_from);

    long number_of_blocks = size / 1024;


    for (int j = 1; j <= number_of_blocks; j++){ //at first, we loop through blocks, reverse them and write to file
        fseek(file_from, -1024*j, SEEK_END); //set cursor
        bytes_read = fread(block_content, sizeof(char), 1024, file_from); //read block
        for (int i = 0; i < bytes_read / 2; i++){ //reverse block
            char temp = block_content[i];
            block_content[i] = block_content[bytes_read - 1 - i];
            block_content[bytes_read - 1 - i] = temp;
        }
        fwrite(block_content, sizeof(char), bytes_read, file_to); //write to output file
    }

    // Then we go to the beginning of our file and read rest of file
    fseek(file_from, 0, SEEK_SET); //set cursor at beginning
    bytes_read = fread(block_content, sizeof(char), size % 1024, file_from); //there will be size % 1024 elements, but to simplify i save it to bytes_read
    for (int i = 0; i < bytes_read / 2; i++){ //reverse rest of file
        char temp = block_content[i];
        block_content[i] = block_content[bytes_read - i - 1];
        block_content[bytes_read - i - 1] = temp;
    }
    fwrite(block_content, sizeof(char), bytes_read, file_to); //write it to file

    fclose(file_from);
    fclose(file_to);
}

int main(int argc, char* argv[]){
    if (argc != 3){
        printf("Not enough or too much arguments!\n");
        return -1;
    }
    printf("Byte after Byte:\n");
    clock_start = times(&start_tms);
    reverse_file_bytes(argv[1], argv[2]);
    clock_end = times(&end_tms);
    print_results(clock_start, clock_end, start_tms, end_tms);
    printf("1024 Blocks:\n");
    clock_start = times(&start_tms);
    reverse_file_blocks(argv[1], argv[2]);
    clock_end = times(&end_tms);
    print_results(clock_start, clock_end, start_tms, end_tms);
    return 0;
}

