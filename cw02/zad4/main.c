#include <stdio.h>
#include <sys/fcntl.h>
#include "timers.h"
#include <dirent.h>
#include <sys/stat.h>
#include <ftw.h>

clock_t clock_start;
clock_t clock_end;
struct tms start_tms;
struct tms end_tms;
long long size = 0;
struct stat path_stat;

int print_info(const char *fpath, const struct stat *sb, int tflag){ //callback function called by ftw for each file or directory
    if (!S_ISDIR(sb->st_mode)){ //check if something is directory
        printf("%s %lld\n", fpath, sb->st_size); //print file name and size
        size += sb->st_size; // add file size to total size of files
    }
    return 0;
}

void search_dir_with_subdirs(char* dir){
    DIR *dir_path;

    int error = ftw(dir, print_info, 1);

    if (error == -1) {
        fprintf(stderr, "[TREE SIZE] FTW error has occured ()\n");
        return;
    }

    printf("Total: %lld", size);
}

int main(int argc, char** argv){
    search_dir_with_subdirs("./");
}

