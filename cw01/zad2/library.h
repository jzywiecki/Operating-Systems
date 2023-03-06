#ifndef LIBRARY_H
#define LIBRARY_H

#include <stdio.h>
#include <libc.h>

typedef struct {
    char **pointers_array; //array of pointers
    int size; //current size of pointers_array
    int max_size; //max size of pointers_array
} Counter;

//function creating Counter structure
Counter* create_counter(int max_size);

//function removing block at given index in Counter structure
void remove_block(Counter* counter, int index);

//function adding block at given index in Counter structure
void add_block(Counter* counter, char* block, int index);

//function that free memory used by Counter
void free_counter(Counter* counter);

//function that gets block content at given index
void get_block_content(Counter* counter, int index);

//function that gets index of the first free space in pointers_array
int get_free_index(Counter* counter);

//auxiliary function that creates command needed for our counting procedure
char* create_command(char* file_name);

//counting procedure for given file name
void counting_procedure(Counter* counter, char* file_name);

#endif //LIBRARY_H
