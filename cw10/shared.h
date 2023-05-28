#ifndef shared_h
#define shared_h

#include <stdbool.h>

#define MAX_CLIENTS 3
#define PROJECT_NUMBER 1
#define MESSAGE_SIZE 250
#define NAME_SIZE 20

//Wspólne oznaczenia
#define TOONE 1 //git
#define TOALL 2 //git
#define LIST 3 //git
#define STOP 4 //git
#define INIT 5 //git
#define SERVER_FULL 6 //git
#define USERNAME_TAKEN 7 //git
#define PING 8 //git


typedef struct Message {
    long message_type; //typ wiadomosci (jak wyzej)
    int client_id; //id klienta
    int other_id; //id przydatne przy wysylaniu do innego klienta
    char message_text[MESSAGE_SIZE]; //wiadomosc
} Message;

typedef struct Client {
    int client_id;
    int fd;
    char name[NAME_SIZE];
    bool is_active;
} Client;


#endif