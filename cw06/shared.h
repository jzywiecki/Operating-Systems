#ifndef commands_h
#define commands_h

#define MAX_CLIENTS 3
#define PROJECT_NUMBER 1
#define MESSAGE_SIZE 250

//Wspólne oznaczenia
//TOONE 1
//TOALL 2
//LIST 3
//STOP 4
//JOIN/INIT 5 (im wyzsze oznaczenie tym wyzszy priorytet bedzie)

typedef struct Message {
    long message_type; //typ wiadomosci (jak wyzej)
    int client_queue_id; //id kolejki klienta
    int client_id; //id klienta
    int other_id; //id przydatne przy wysylaniu do innego klienta
    key_t client_queue_key; //klucz kolejki klienta
    char message_text[MESSAGE_SIZE]; //wiadomosc
} Message;


#endif