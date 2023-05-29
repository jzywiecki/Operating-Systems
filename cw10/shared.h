#ifndef shared_h
#define shared_h

#include <stdbool.h>

#define MAX_CLIENTS 3
#define MESSAGE_SIZE 250
#define NAME_SIZE 20

//Wspólne oznaczenia
#define TOONE 1 //zrealizowane
#define TOALL 2 //zrealizowane
#define LIST 3 //zrealizowane
#define STOP 4 //zrealizowane
#define INIT 5 //zrealizowane
#define SERVER_FULL 6 //zrealizowane
#define USERNAME_TAKEN 7 //zrealizowane
#define PING 8 //zrealizowane


typedef struct Message {
    long message_type; //typ wiadomosci (jak wyzej)
    int client_id; //id klienta
    int other_id; //id przydatne przy wysylaniu do innego klienta
    char message_text[MESSAGE_SIZE]; //wiadomosc
} Message;

typedef struct Client {
    int client_id; //id klienta
    int fd; //deskryptor
    char name[NAME_SIZE]; //nazwa klienta
    bool is_active; //czy jest aktywny
} Client;



#endif