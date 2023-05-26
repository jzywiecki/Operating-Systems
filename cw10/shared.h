#ifndef shared_h
#define shared_h

#define MAX_CLIENTS 3
#define PROJECT_NUMBER 1
#define MESSAGE_SIZE 250

//Wspólne oznaczenia
#define TOONE 1
#define TOALL 2
#define LIST 3
#define STOP 4
#define INIT 5 //(im wyzsze oznaczenie tym wyzszy priorytet bedzie)
#define DISCONNECT 6
#define SERVER_FULL 7
#define USERNAME_TAKEN 8
#define PING 9


typedef struct Message {
    long message_type; //typ wiadomosci (jak wyzej)
    int client_id; //id klienta
    int other_id; //id przydatne przy wysylaniu do innego klienta
    char message_text[MESSAGE_SIZE]; //wiadomosc
} Message;


#endif