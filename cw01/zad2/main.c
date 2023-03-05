#include "library.h"
#include <limits.h>
#include <sys/times.h>


Counter *counter;
char* sep = " \n";

long double time_sec(clock_t t1, clock_t t2){
    return (long double)(t2 - t1) / sysconf(_SC_CLK_TCK);
}

void print_results(clock_t clock_start, clock_t clock_end, struct tms start_tms, struct tms end_tms){
    printf("real time: %Lf\n", time_sec(clock_start, clock_end));
    printf("user time: %Lf\n", time_sec(start_tms.tms_utime, end_tms.tms_utime));
    printf("sys time: %Lf\n\n", time_sec(start_tms.tms_stime, end_tms.tms_stime));
}

clock_t clock_start;
clock_t clock_end;
struct tms start_tms;
struct tms end_tms;

void init_handler(char* operator){
    char *end;
    operator = strtok(NULL, sep);
    int size = strtol(operator, &end, 10);
    if (end==operator){
        printf("Invalid init input!\n");
    }
    else {
        clock_start = times(&start_tms);
        create_counter(size);
        clock_end = times(&end_tms);
        print_results(clock_start, clock_end, start_tms, end_tms);
    }

}

void count_handler(char* operator){
    clock_start = times(&start_tms);
    operator = strtok(NULL, sep);
    counting_procedure(counter, operator);
    clock_end = times(&end_tms);
    print_results(clock_start, clock_end, start_tms, end_tms);

}

void show_handler(char* operator){
    char *end;
    operator = strtok(NULL, sep);
    int index = strtol(operator, &end, 10);
    if (end==operator){
        printf("Invalid index input!\n");
    }
    else {
        clock_start = times(&start_tms);
        get_block_content(index);
        clock_end = times(&end_tms);
        print_results(clock_start, clock_end, start_tms, end_tms);

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
        clock_start = times(&start_tms);
        remove_block(index);
        clock_end = times(&end_tms);
        print_results(clock_start, clock_end, start_tms, end_tms);

    }
}

void destroy_handler(){
    clock_start = times(&start_tms);
    free_pointers_array();
    clock_end = times(&end_tms);
    print_results(clock_start, clock_end, start_tms, end_tms);

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

