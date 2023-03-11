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
    DIR *dir_path;
    long long size = 0; //it should be off_t
    struct dirent *current_position;
    struct stat path_stat;

    if((dir_path = opendir(dir)) == NULL) {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    while((current_position = readdir(dir_path)) != NULL) {
        stat(current_position->d_name, &path_stat);
        if(!S_ISDIR(path_stat.st_mode)){
            size += path_stat.st_size;
            printf("%s: %lld bytes\n", current_position->d_name, path_stat.st_size);
        }
        else{
            continue;
        }
    }
    printf("Total: %lld", size);
}



int main(){
    search_dir("./");
}

