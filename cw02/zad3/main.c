#include <stdio.h>
#include <sys/fcntl.h>
#include "timers.h"
#include <dirent.h>
#include <sys/stat.h>

clock_t clock_start;
clock_t clock_end;
struct tms start_tms;
struct tms end_tms;

void search_dir(char* dir){
    DIR *dir_path; //declare directory
    long long size = 0;
    struct dirent *current_position; //structure that allows us to read and manipulate directory entries
    struct stat path_stat; //structure to get information about file

    dir_path = opendir(dir);
    if(dir_path == NULL) { //simple error holding
        printf("Could not open directory!\n");
        return;
    }

    while((current_position = readdir(dir_path)) != NULL) { //while there are objects in our directory
        stat(current_position->d_name, &path_stat); //get stats about file
        if(!S_ISDIR(path_stat.st_mode)){ //if file is not directory
            size += path_stat.st_size; //add size to total size
            printf("%s: %lld bytes\n", current_position->d_name, path_stat.st_size); //print info
        }
        else{
            continue;
        }
    }
    printf("Total: %lld\n", size);
}



int main(int argc, char* argv[]){
    if (argc != 2){
        printf("Not enough or too much arguments!\n");
        return -1;
    }
    clock_start = times(&start_tms);

    search_dir(argv[1]);

    clock_end = times(&end_tms);
    print_results(clock_start, clock_end, start_tms, end_tms);
    return 0;
}

