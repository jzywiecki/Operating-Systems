#ifndef commands_h
#define commands_h

#define MAX_CLIENTS 3
#define PROJECT_NUMBER 1
#define MESSAGE_SIZE 250


typedef struct Message {
    long message_type;
    int client_queue_id;
    key_t client_queue_key;
    char message_text[MESSAGE_SIZE];
} Message;


#define LIST 1
#define TOALL 2
#define TOONE 3
#define STOP 4

#endif