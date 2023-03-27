#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>


void handler(int signum){
    printf("Recieved signal: %d", signum);
}


int main(int argc, char* argv[]) {


}
