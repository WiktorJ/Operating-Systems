#ifndef CONFIG
#define CONFIG

#define MAX_MESSAGE_SIZE 4096
#define MAX_CLIENT_NAME_LENGTH 128
#define MAX_CLIENTS_NUMBER 10
#define CHAT_KEY 1
#define CHAT_PATH "."
#define CLIENT_TO_SERVER 1
#define SERVER_TO_CLIENT 2
struct client{
	long message_type;
	int queue;
	char name[MAX_CLIENT_NAME_LENGTH];
};

struct message{
	long type;
	char message[MAX_MESSAGE_SIZE]; 
};

#endif
