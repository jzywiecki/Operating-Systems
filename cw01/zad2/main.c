#include "../zad1/library.h"
#include <limits.h>
#include <sys/times.h>


Counter *counter;
char* sep = " \n";

void init_handler(char* operator){
    char *end;
    operator = strtok(NULL, sep);
    int size = strtol(operator, &end, 10);
    if (end==operator){
        printf("Invalid init input!\n");
    }
    else {
        create_counter(size);
    }

}

void count_handler(char* operator){
    operator = strtok(NULL, sep);
    counting_procedure(counter, operator);
}

void show_handler(char* operator){
    char *end;
    operator = strtok(NULL, sep);
    int index = strtol(operator, &end, 10);
    if (end==operator){
        printf("Invalid index input!\n");
    }
    else {
        get_block_content(index);
    }
}

void delete_handler(char* operator){
    char *end;
    operator = strtok(NULL, sep);
    int index = strtol(operator, &end, 10);
    if (end==operator){
        printf("Invalid index input!\n");
    }
    else {
        remove_block(index);
    }
}

void destroy_handler(){
    free_pointers_array();
}

int main() {
    char line[LINE_MAX];
    while(1){
        fgets(line, LINE_MAX, stdin);
        char* operator = strtok(line, sep);

        if (strcmp(operator, "init") == 0){
            init_handler(operator);
        }
        else if (strcmp(operator, "count") == 0){
            count_handler(operator);
        }
        else if (strcmp(operator, "show") == 0){
            show_handler(operator);
        }
        else if (strcmp(operator, "delete") == 0){
            delete_handler(operator);
        }
        else if (strcmp(operator, "destroy") == 0){
            destroy_handler();
        }
        else if(strcmp(operator, "quit") == 0){
            exit(0);
        }
        else{
            printf("Command could not be recognised. Check spelling and try again!\n");
        }
    }
}

