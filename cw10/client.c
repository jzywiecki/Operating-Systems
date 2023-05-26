
#include <sys/socket.h>
#include <string.h>
#include <printf.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "shared.h"

int create_local_socket(char* path){
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);

    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1){
        printf("Error while creating socket!\n");
        exit(1);
    }
    connect(sock, (struct sockaddr*) &addr, sizeof addr);
    return sock;
}


int create_inet_socket(char* ipv4, int port){
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ipv4, &addr.sin_addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1){
        printf("Error while creating socket!\n");
        exit(1);
    }
    connect(sock, (struct sockaddr*) &addr, sizeof addr);
    return sock;
}


int sock;
void SIGINT_handler(int signum){
    Message msg;
    msg.message_type = DISCONNECT;
    send(sock, &msg, sizeof msg, 0);
    exit(0);
}


int main(int argc, char** argv) {
    if (argc != 4 && argc != 5){
        printf("Wrong number of arguments!\n");
        exit(1);
    }

    char* client_name = argv[1];
    char* connection_type = argv[2];
    char* address = argv[3];
    char* port = NULL;
    if (argc == 5){
        port = argv[4];
    }

    signal(SIGINT, SIGINT_handler);

    if (strcmp(connection_type, "local") == 0){
        sock = create_local_socket(address);
    }
    else if (strcmp(connection_type, "inet") == 0){
        sock = create_inet_socket(address, atoi(port));
    }
    else{
        printf("Wrong connection type!\n");
        exit(1);
    }

    Message msg;
    msg.message_type = INIT;
    msg.client_id = getpid();
    strcpy(msg.message_text, client_name);
    send(sock, &msg, sizeof msg, 0);

    int epoll = epoll_create1(0);


    return 0;
}

