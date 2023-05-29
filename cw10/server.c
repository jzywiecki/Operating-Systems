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

pthread_mutex_t clients_mutex; //mutex do synchronizacji dostepu do tablicy klientow
Client clients[MAX_CLIENTS]; //tablica klientow

void* ping(void* arg){ //funkcja wysylajaca pingi do klientow wywolana w osobnym wątku
    Message msg;
    msg.message_type = PING; //ustawiamy typ wiadomosci na ping
    while (1){
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++){ //dla kazdego klienta
            if (clients[i].is_active){
                send(clients[i].fd, &msg, sizeof msg, 0);
                clients[i].is_active = false; //ustawiamy flage aktywnosci na false, potem przy odbiorze wiadomosci ustawiamy na true z powrotem
            }
            else{
                clients[i].fd = -1; //jesli nie jest aktywny, to sie go pozbywamy
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

    int port = atoi(argv[1]); //uzupełniamy zmienne z argumentow
    char* socket_path = argv[2];

    pthread_mutex_init(&clients_mutex, NULL); //inicjalizujemy mutex

    int kq = kqueue(); //tworzymy kqueue
    if (kq == -1){
        printf("Error while creating kqueue!\n");
        exit(1);
    }

    int sock_unix = socket(AF_UNIX, SOCK_STREAM, 0); //tworzymy sockety
    if (sock_unix == -1){
        printf("Error while creating socket!\n");
        exit(1);
    }

    int sock_inet = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_inet == -1){
        printf("Error while creating socket!\n");
        exit(1);
    }

    struct sockaddr_un addr_local; //tworzymy struktury adresowe dla socketa unixowego i inetowego
    addr_local.sun_family = AF_UNIX;
    strcpy(addr_local.sun_path, socket_path);
    unlink(socket_path);
    socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_in addr_inet;
    addr_inet.sin_family = AF_INET;
    addr_inet.sin_port = htons(port);
    addr_inet.sin_addr.s_addr = INADDR_ANY;

    bind(sock_unix, (struct sockaddr*) &addr_local, sizeof addr_local); //bindujemy sockety
    bind(sock_inet, (struct sockaddr*) &addr_inet, sizeof addr_inet);

    listen(sock_unix, MAX_CLIENTS); //ustawiamy sockety na nasluchiwanie
    listen(sock_inet, MAX_CLIENTS);

    pthread_t ping_thread; //tworzymy wątek do wysylania pingow
    pthread_create(&ping_thread, NULL, ping, NULL); //uruchamiamy watek
    struct kevent event_inet, event_unix; //tworzymy eventy dla socketa unixowego i inetowego
    EV_SET(&event_unix, sock_unix, EVFILT_READ, EV_ADD, 0, 0, NULL); //dodajemy sockety do kqueue
    EV_SET(&event_inet, sock_inet, EVFILT_READ, EV_ADD, 0, 0, NULL);

    //dodajemy sockety do kqueue
    if (kevent(kq, &event_unix, 1, NULL, 0, NULL) == -1 || kevent(kq, &event_inet, 1, NULL, 0, NULL) == -1) {
        perror("Adding server socket to kqueue failed");
        close(sock_unix);
        close(kq);
        exit(EXIT_FAILURE);
    }

    struct kevent events[MAX_CLIENTS + 2]; //tworzymy tablice eventow
    printf("Server is running!\n");
    while (1) {
        int nev = kevent(kq, NULL, 0, events, MAX_CLIENTS + 2, NULL); //czekamy na eventy
        if (nev == -1) {
            printf("Error while waiting for events!\n");
            exit(1);
        }
        for (int i = 0; i < nev; i++) { //dla kazdego eventu
            if (events[i].ident == sock_unix || events[i].ident == sock_inet) { //jesli event dotyczy socketa serwera
                int client_fd = accept(events[i].ident, NULL, NULL); //akceptujemy polaczenie
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
            } else { //jesli event dotyczy klienta
                int client_fd = events[i].ident;
                Message msg;
                recv(client_fd, &msg, sizeof msg, 0); //odbieramy wiadomosc
                if (msg.message_type == INIT) { //jesli to INIT
                    printf("Nowy klient!\n");
                    pthread_mutex_lock(&clients_mutex);
                    int client_id = -1;
                    bool name_taken = false; //sprawdzamy czy nazwa jest zajeta
                    for (int j = 0; j < MAX_CLIENTS; j++) { //szukamy wolnego miejsca
                        if (!clients[j].is_active) {
                            client_id = j;
                            break;
                        }
                        if (clients[j].is_active && strcmp(clients[j].name, msg.message_text) == 0) {
                            name_taken = true;
                            break;
                        }
                    }
                    if (name_taken) { //jesli nazwa zajeta to wysylamy wiadomosc i zamykamy polaczenie
                        Message msg;
                        msg.message_type = USERNAME_TAKEN;
                        send(client_fd, &msg, sizeof msg, 0);
                        close(client_fd);
                    }
                    if (client_id == -1) { //jesli nie ma miejsca to wysylamy wiadomosc i zamykamy polaczenie
                        Message msg;
                        msg.message_type = SERVER_FULL;
                        send(client_fd, &msg, sizeof msg, 0);
                        close(client_fd);
                    } else { //jesli jest miejsce to dodajemy klienta do tablicy klientow
                        clients[client_id].client_id = client_id;
                        clients[client_id].fd = client_fd;
                        clients[client_id].is_active = true;
                        strcpy(clients[client_id].name, msg.message_text);
                        struct kevent event;
                        EV_SET(&event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                        kevent(kq, &event, 1, NULL, 0, NULL); //dodajemy klienta do kqueue
                    }

                    pthread_mutex_unlock(&clients_mutex);
                }
                else if (msg.message_type == LIST){ //jesli to LIST
                    printf("LIST\n");
                    Message msg;
                    msg.message_text[0] = '\0'; //czyscimy wiadomosc
                    msg.message_type = LIST;
                    pthread_mutex_lock(&clients_mutex);
                    for (int j = 0; j < MAX_CLIENTS; j++){ //dodajemy do wiadomosci nazwy klientow
                        printf("%d\n", clients[j].is_active);
                        if (clients[j].is_active){
                            strcat(msg.message_text, clients[j].name);
                            strcat(msg.message_text, "  ");
                        }
                    }
                    strcat(msg.message_text, "\n"); //dodajemy znak konca linii
                    send(client_fd, &msg, sizeof msg, 0); //wysylamy wiadomosc do klienta, ktory jej zażądał
                    pthread_mutex_unlock(&clients_mutex);
                }
                else if (msg.message_type == TOALL){ //jesli to TOALL
                    printf("TOALL\n");
                    pthread_mutex_lock(&clients_mutex);
                    for (int j = 0; j < MAX_CLIENTS; j++){ //wysylamy wiadomosc do wszystkich klientow
                        if (clients[j].is_active){
                            send(clients[j].fd, &msg, sizeof msg, 0);
                        }
                    }
                    pthread_mutex_unlock(&clients_mutex);
                }
                else if (msg.message_type == TOONE){ //jesli to TOONE
                    printf("TOONE\n");
                    pthread_mutex_lock(&clients_mutex);
                    for (int j = 0; j < MAX_CLIENTS; j++){ //wysylamy wiadomosc do wybranego klienta
                        if (clients[j].is_active && (clients[j].client_id == msg.other_id)){
                            send(clients[j].fd, &msg, sizeof msg, 0);
                        }
                    }
                    pthread_mutex_unlock(&clients_mutex);
                }
               else if (msg.message_type == STOP){ //jesli to STOP
                    printf("STOP\n");
                    pthread_mutex_lock(&clients_mutex);
                    for (int j = 0; j < MAX_CLIENTS; j++){ //usuwanie klienta z tablicy klientow
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
                    EV_SET(&event, client_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL); //usuwanie klienta z kqueue
                    kevent(kq, &event, 1, NULL, 0, NULL); //usuwanie klienta z kqueue

               } else if (msg.message_type == PING){ //jesli to PING
                    printf("PING\n");
                    pthread_mutex_lock(&clients_mutex);
                    for (int j = 0; j < MAX_CLIENTS; j++){ //odswiezanie klienta
                        if (clients[j].fd == client_fd){
                            clients[j].is_active = true;
                            break;
                        }
                    }
                    pthread_mutex_unlock(&clients_mutex);
               }
            }
        }
    }
    return 0;
}
