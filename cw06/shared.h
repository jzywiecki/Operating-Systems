#ifndef commands_h
#define commands_h

#define MAX_CLIENTS 3
#define PROJECT_NUMBER 1
#define MESSAGE_SIZE 250

//Wspólne oznaczenia
//LIST 1
//TOALL 2
//TOONE 3
//STOP 4
//JOIN/INIT 5

typedef struct Message {
    long message_type;
    int client_queue_id;
    int client_id;
    int other_id;
    key_t client_queue_key;
    char message_text[MESSAGE_SIZE];
} Message;


#endif