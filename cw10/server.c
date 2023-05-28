#include <printf.h>
#include <stdlib.h>
#include <sys/event.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include "shared.h"

pthread_mutex_t clients_mutex;
Client clients[MAX_CLIENTS];

void* ping(void* arg){
    Message msg;
    msg.message_type = PING;
    while (1){
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++){
            if (clients[i].is_active){
                send(clients[i].fd, &msg, sizeof msg, 0);
                clients[i].is_active = false;
            }
            else{
                clients[i].fd = -1;
                clients[i].client_id = -1;
                clients[i].name[0] = '\0';
                close(clients[i].fd);
            }
        }
        pthread_mutex_unlock(&clients_mutex);
        sleep(5);
    }
    return 0;
}


int main(int argc, char** argv) {
    if (argc != 3){
        printf("Wrong number of arguments!\n");
        exit(1);
    }

    int port = atoi(argv[1]);
    char* socket_path = argv[2];

    pthread_mutex_init(&clients_mutex, NULL);

    int kq = kqueue();
    if (kq == -1){
        printf("Error while creating kqueue!\n");
        exit(1);
    }

    int sock_unix = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_unix == -1){
        printf("Error while creating socket!\n");
        exit(1);
    }

    int sock_inet = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_inet == -1){
        printf("Error while creating socket!\n");
        exit(1);
    }

    struct sockaddr_un addr_local;
    addr_local.sun_family = AF_UNIX;
    strcpy(addr_local.sun_path, socket_path);
    unlink(socket_path);
    socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_in addr_inet;
    addr_inet.sin_family = AF_INET;
    addr_inet.sin_port = htons(port);
    addr_inet.sin_addr.s_addr = INADDR_ANY;

    bind(sock_unix, (struct sockaddr*) &addr_local, sizeof addr_local);
    bind(sock_inet, (struct sockaddr*) &addr_inet, sizeof addr_inet);

    listen(sock_unix, MAX_CLIENTS);
    listen(sock_inet, MAX_CLIENTS);

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, ping, NULL);
    struct kevent events[MAX_CLIENTS + 2];
    struct kevent event_inet, event_unix;
    EV_SET(&event_unix, sock_unix, EVFILT_READ, EV_ADD, 0, 0, NULL);
    EV_SET(&event_inet, sock_inet, EVFILT_READ, EV_ADD, 0, 0, NULL);

    if (kevent(kq, &event_unix, 1, NULL, 0, NULL) == -1 || kevent(kq, &event_inet, 1, NULL, 0, NULL) == -1) {
        perror("Adding server socket to kqueue failed");
        close(sock_unix);
        close(kq);
        exit(EXIT_FAILURE);
    }

    printf("Server is running!\n");
    while (1) {
        int nev = kevent(kq, NULL, 0, events, MAX_CLIENTS + 1, NULL);
        printf("After kevent\n");
        if (nev == -1) {
            printf("Error while waiting for events!\n");
            exit(1);
        }
        for (int i = 0; i < nev; i++) {
            if (events[i].ident == sock_unix || events[i].ident == sock_inet) {
                int client_fd = accept(events[i].ident, NULL, NULL);
                if (client_fd == -1) {
                    printf("Error while accepting connection!\n");
                    exit(1);
                }
                struct kevent event;
                EV_SET(&event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
                    perror("Adding client socket to kqueue failed");
                    close(client_fd);
                    exit(EXIT_FAILURE);
                }
            } else {
                int client_fd = events[i].ident;
                Message msg;
                recv(client_fd, &msg, sizeof msg, 0);
                if (msg.message_type == INIT) {
                    printf("Nowy klient!\n");
                    pthread_mutex_lock(&clients_mutex);
                    int client_id = -1;
                    bool name_taken = false;
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (!clients[j].is_active) {
                            client_id = j;
                            break;
                        }
                        if (clients[j].is_active && strcmp(clients[j].name, msg.message_text) == 0) {
                            name_taken = true;
                            break;
                        }
                    }
                    if (name_taken) {
                        Message msg;
                        msg.message_type = USERNAME_TAKEN;
                        send(client_fd, &msg, sizeof msg, 0);
                        close(client_fd);
                    }
                    if (client_id == -1) {
                        Message msg;
                        msg.message_type = SERVER_FULL;
                        send(client_fd, &msg, sizeof msg, 0);
                        close(client_fd);
                    } else {
                        clients[client_id].client_id = client_id;
                        clients[client_id].fd = client_fd;
                        clients[client_id].is_active = true;
                        strcpy(clients[client_id].name, msg.message_text);
                        struct kevent event;
                        EV_SET(&event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                        kevent(kq, &event, 1, NULL, 0, NULL);
                    }

                    pthread_mutex_unlock(&clients_mutex);
                }
                else if (msg.message_type == LIST){
                    printf("LIST\n");
                    Message msg;
                    //clear message text
                    msg.message_text[0] = '\0';
                    msg.message_type = LIST;
                    pthread_mutex_lock(&clients_mutex);
                    for (int j = 0; j < MAX_CLIENTS; j++){
                        printf("%d\n", clients[j].is_active);
                        if (clients[j].is_active){
                            strcat(msg.message_text, clients[j].name);
                            strcat(msg.message_text, "  ");
                        }
                    }
                    strcat(msg.message_text, "\n");
                    send(client_fd, &msg, sizeof msg, 0);
                    printf("After send\n");
                    pthread_mutex_unlock(&clients_mutex);

                }
                else if (msg.message_type == TOALL){
                    printf("TOALL\n");
                    pthread_mutex_lock(&clients_mutex);
                    for (int j = 0; j < MAX_CLIENTS; j++){
                        if (clients[j].is_active){
                            send(clients[j].fd, &msg, sizeof msg, 0);
                        }
                    }
                    pthread_mutex_unlock(&clients_mutex);
                }
                else if (msg.message_type == TOONE){
                    printf("TOONE\n");
                    pthread_mutex_lock(&clients_mutex);
                    for (int j = 0; j < MAX_CLIENTS; j++){
                        if (clients[j].is_active && (clients[j].client_id == msg.other_id)){
                            send(clients[j].fd, &msg, sizeof msg, 0);
                        }
                    }
                    pthread_mutex_unlock(&clients_mutex);
                }
               else if (msg.message_type == STOP){
                    printf("STOP\n");
                    pthread_mutex_lock(&clients_mutex);
                    for (int j = 0; j < MAX_CLIENTS; j++){
                        if (clients[j].is_active && clients[j].fd == client_fd){
                            clients[j].is_active = false;
                            clients[j].fd = -1;
                            clients[j].client_id = -1;
                            clients[j].name[0] = '\0';
                            close(clients[j].fd);
                            break;
                        }
                    }
                    pthread_mutex_unlock(&clients_mutex);
                    struct kevent event;
                    EV_SET(&event, client_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(kq, &event, 1, NULL, 0, NULL);

               } else if (msg.message_type == PING){
                    printf("PING\n");
                    pthread_mutex_lock(&clients_mutex);
                    for (int j = 0; j < MAX_CLIENTS; j++){
                        if (clients[j].fd == client_fd){
                            clients[j].is_active = true;
                            break;
                        }
                    }


                    pthread_mutex_unlock(&clients_mutex);

               }
            }
        }
        printf("After for\n");
    }

    return 0;
}
