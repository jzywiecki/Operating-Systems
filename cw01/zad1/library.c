#include "library.h"
#include <stdio.h>
#include <libc.h>
#include <unistd.h>

Counter* create_counter(int max_size){
    if (max_size <= 0){ //check if max_size of our structure is at least 1
        printf("Array length should be greater than 0!\n");
        return NULL;
    }
    Counter *counter;
    counter = malloc(sizeof(Counter)); //alloc memory for our structure
    counter->max_size = max_size;
    counter->size = 0;
    counter->pointers_array = calloc(max_size, sizeof(char*)); //alloc memory for array of pointers
    printf("Counter created successfully!\n");
    return counter;
}


void remove_block(Counter* counter, int index){
    if (counter == NULL){ //check if structure exists
        printf("Structure is not made yet!\n");
        return;
    }
    if( index < 0 || index >= counter->max_size) //check if index is proper
    {
        printf("Indexes of structure are above 0 and under max_size!\n");
        return;
    }
    if (counter->pointers_array[index] == NULL){ //check if there is block saved at given index
        printf("There is no block saved at given index!\n");
        return;
    }
    if (counter->size == 0){ //check if array is empty
        printf("Array is empty!\n");
        return;
    }
    counter->size--;
    free(counter->pointers_array[index]);
    counter->pointers_array[index] = NULL;
    printf("Successfully removed block at given index!\n");
}

void add_block(Counter* counter, char* block, int index){
    if (counter == NULL){ //check if structure exists
        printf("Structure is not made yet!\n");
        return;
    }
    if (index < 0 || index >= counter->max_size) {
        printf("Indexes of structure are above 0 and under max_size!\n"); //check if index is proper
        return;
    }
    if (counter->size == counter->max_size){ //check if array is full
        printf("Array is full!\n");
        return;
    }
    counter->size++;
    counter->pointers_array[index] = (char *) calloc(strlen(block) + 1, sizeof(char));
    strcpy(counter->pointers_array[index], block);
    printf("Saved!\n");
}

void free_counter(Counter* counter){
    if (counter == NULL){ //check if structure exists
        printf("Structure is not made yet, so you can't delete it.\n");
        return;
    }
    for (int i = 0; i < counter->max_size; i++){ //free all pointers of pointers_array
        if (counter->pointers_array[i] != NULL){
            free(counter->pointers_array[i]);
        }
    }
    free(counter->pointers_array); //free pointers_array
    free(counter); //free the whole structure
    printf("Action completed successfully!\n");
    counter = NULL;
}

void get_block_content(Counter* counter, int index){
    if (counter == NULL){ //check if structure exists
        printf("Structure is not made yet!\n");
        return;
    }
    else if(index < 0 || index >= counter->max_size){ // checks if index is proper
        printf("Indexes of structure are above 0 and under max_size!\n");
        return;
    }
    else if (counter->pointers_array[index] == NULL){ //checks if there is saved block at given index
        printf("Block at given index is empty!\n");
        return;
    }
    else {
        printf("%s\n", counter->pointers_array[index]);
    }
}

int get_free_index(Counter* counter){
    if (counter == NULL){ //check if structure exists
        printf("Structure is not made yet!\n");
        return -1;
    }
    if (counter->size == counter->max_size){ //check if array is full
        printf("The array is full!\n");
        return -1;
    }
    for (int i = 0; i < counter->max_size; i++){
        if (counter->pointers_array[i] == NULL){
            printf("Found free index (%d)!\n", i);
            return i;
        }
    }
    return -1;
}


char* create_command(char* file_name){
    char* beginning = "wc ";
    char* tmp_file = " > tmp/fileAnalysis.tmp";

    //count size of the whole command
    unsigned long size = strlen(beginning) + strlen(tmp_file) + strlen(file_name) + 1;

    //alloc size for the command
    char* command = malloc(size);

    //copy beginning to the command
    strcpy(command, beginning);

    //add file_name "wc file_name\0"
    strcat(command, file_name);

    //add tmp file we will save result at "wc file_name >> tmp/fileAnalysis.tmp\0"
    strcat(command, tmp_file);

    return command;
}

void counting_procedure(Counter* counter, char* file_name){
    //check if structure is already made and if it has empty place to store new data
    if (counter == NULL){
        printf("Structure is not made yet.\n");
        return;
    }
    if (counter->size == counter->max_size){
        printf("Structure is already full.\n");
        return;
    }
    if (access(file_name, F_OK) != 0){
        printf("File does not exist!\n");
        return;
    }

    //making tmp dir
    system("mkdir tmp");

    //running wc on file and saving it to tmp/fileAnalysis.tmp
    system(create_command(file_name));

    //opening file in read mode
    FILE* file;
    file = fopen("tmp/fileAnalysis.tmp", "r");

    //here we count chars of our file analysis
    int count_chars = 0;
    int c; //fgetc() returns int
    while ((c = fgetc(file))){
        if (c == EOF){
            break;
        }
        count_chars++;
    }

    //go back to file beginning
    rewind(file);

    //calloc memory for file content
    char* file_content;
    file_content = calloc(count_chars+1, sizeof(char));

    //save file content to our char* file_content
    fgets(file_content, count_chars, file);

    //close file
    fclose(file);

    //removing tmp dir and files inside
    system("rm -r -f tmp");

    //adding block about our analysed file at free index
    int index = get_free_index(counter);
    if (index != -1) {
        add_block(counter, file_content, index);
    }
    else{
        printf("Error in finding free index in pointer array!\n");
    }
}