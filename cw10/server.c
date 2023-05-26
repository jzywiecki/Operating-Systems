#include <printf.h>
#include <stdlib.h>


void ping(){

}


int main(int argc, char** argv) {
    if (argc != 3){
        printf("Wrong number of arguments!\n");
        exit(1);
    }

    int port = atoi(argv[1]);
    char* socket_path = argv[2];



    return 0;

}