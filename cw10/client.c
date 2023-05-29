#include <sys/socket.h>
#include <string.h>
#include <printf.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/event.h>
#include "shared.h"

int sock; //deskryptor gniazda

void handle_sigint(int signum){ //obsluga SIGINT, w takim przypadku wysylamy sygnal o zastopowaniu naszego klienta
    Message msg;
    msg.message_type = STOP;
    send(sock, &msg, sizeof msg, 0);
    exit(0);
}

int main(int argc, char** argv) {
    setbuf(stdout, NULL); //aby wszystko sie wypisywalo od razu
    if (argc != 4 && argc != 5){ //sprawdzamy czy podano odpowiednia liczbe argumentow
        printf("Wrong number of arguments!\n");
        exit(1);
    }

    char* client_name = argv[1]; //uzupełniamy zmienne z argumentow
    char* connection_type = argv[2];
    char* address = argv[3];
    char* port = NULL;
    if (argc == 5){
        port = argv[4];
    }

    signal(SIGINT, handle_sigint); //ustawiamy obsluge SIGINT

    if (strcmp(connection_type, "local") == 0){ //sprawdzamy typ polaczenia i laczymy sie z serwerem
        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, address);
        sock = socket(AF_UNIX, SOCK_STREAM, 0); //socket
        if (sock == -1){
            printf("Error while creating socket!\n");
            exit(1);
        }
        if (connect(sock, (struct sockaddr*) &addr, sizeof addr)) { //connect
             perror("Error while connecting to local socket!\n");
             close(sock);
             exit(1);
        }
        printf("Connected to local socket!\n");
    }
    else if (strcmp(connection_type, "inet") == 0){
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(atoi(port));
        inet_pton(AF_INET, address, &addr.sin_addr);
        sock = socket(AF_INET, SOCK_STREAM, 0); //socket
        if (sock == -1){
            printf("Error while creating socket!\n");
            exit(1);
        }
        if (connect(sock, (struct sockaddr*) &addr, sizeof addr) == -1){ //connect
            printf("Error while connecting to inet socket!\n");
            close(sock);
            exit(1);
        }
        printf("Connected to inet socket!\n");
    }
    else{
        printf("Wrong connection type!\n");
        exit(1);
    }

    Message msg; //wysylamy wiadomosc INIT, aby poinformować serwer o połączeniu
    msg.message_type = INIT;
    strcpy(msg.message_text, client_name);
    if (send(sock, &msg, sizeof msg, 0) == -1){
        printf("Error while sending INIT message!\n");
    }

    int kq = kqueue(); //tworzymy kqueue
    struct kevent event_stdin, event_sock; //tworzymy zdarzenia dla stdin i socketa
    EV_SET(&event_stdin, STDIN_FILENO, EVFILT_READ, EV_ADD, 0, 0, NULL); //dodajemy zdarzenia do kolejki
    kevent(kq, &event_stdin, 1, NULL, 0, NULL); //rejestrujemy zdarzenia
    EV_SET(&event_sock, sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(kq, &event_sock, 1, NULL, 0, NULL);

    struct kevent events[2]; //tworzymy tablice na zdarzenia
    while (1) {
        kevent(kq, NULL, 0, events, 2, NULL); //czekamy na zdarzenia
        for (int i = 0; i < 2; i++) {
            if (events[i].filter == EVFILT_READ) { //sprawdzamy czy zdarzenie jest typu READ
                if (events[i].ident == STDIN_FILENO) { //sprawdzamy czy zdarzenie dotyczy stdin czy socketa, tutaj stdin
                    char *line = NULL;
                    size_t len = 0;
                    getline(&line, &len, stdin);
                    if (strcmp(line, "LIST\n") == 0) { //w zaleznosci od komendy wysylamy odpowiednia wiadomosc
                        msg.message_type = LIST;
                        send(sock, &msg, sizeof msg, 0);
                    } else if (strcmp(line, "STOP\n") == 0) {
                        msg.message_type = STOP;
                        send(sock, &msg, sizeof msg, 0);
                        exit(0);
                    } else if (strncmp(line, "TOALL", 5) == 0) {
                        msg.message_type = TOALL;
                        strcpy(msg.message_text, line + 6);
                        send(sock, &msg, sizeof msg, 0);
                    } else if (strncmp(line, "TOONE", 5) == 0) {
                        msg.message_type = TOONE;
                        msg.other_id = atoi(line + 6);
                        strcpy(msg.message_text, line + 8);
                        send(sock, &msg, sizeof msg, 0);
                    } else {
                        printf("Wrong command!\n");
                    }
                } else if (events[i].ident == sock) { //jezeli zdarzenie dotyczy socketa to odbieramy wiadomosc i ja obslugujemy
                    recv(sock, &msg, sizeof msg, 0);
                    if (msg.message_type == STOP) {
                        printf("Server stopped!\n");
                        exit(0);
                    }
                    if (msg.message_type == INIT){
                        printf("Connected to server!\n");
                        int client_id = msg.client_id;
                    }else if (msg.message_type == SERVER_FULL) {
                        printf("Server is full!\n");
                        exit(1);
                    } else if (msg.message_type == USERNAME_TAKEN) {
                        printf("Username is taken!\n");
                        exit(1);
                    } else if (msg.message_type == PING) {
                        send(sock, &msg, sizeof msg, 0);
                    } else {
                        printf("%s", msg.message_text);
                    }
                }
            }
        }
    }
    return 0;
}
