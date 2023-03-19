#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>




int main(int argc, char * argv[]){
    if (argc < 2){
        printf("Not enaugh arguments!\n");
        exit(EXIT_FAILURE);
    }
    if (argc > 2){
        printf("Too many arguments!\n");
        exit(EXIT_FAILURE);
    }

    int number_of_processes = atoi(argv[1]);

    for (int i = 0; i < number_of_processes; i++){
        pid_t pid = fork();
        if (pid == 0){
            printf("Parent: %d, child: %d\n", getppid(), getpid());
            exit(EXIT_SUCCESS);
        }
        else{
            waitpid(pid, NULL, 0);
        }
    }

    printf("%s\n", argv[1]);
    return 0;
}

