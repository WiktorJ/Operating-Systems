#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "chat.h"
#include <time.h>
void print_error(int error, char* message){
	puts(message);
	puts(strerror(error));
}

void init_clients(struct client *clients);
void send_message(char *m, int do_not_send_to, struct client *clients);

int main(int argc, char *argv[]){
	key_t key; 
	if( (key = ftok(CHAT_PATH,CHAT_KEY)) == -1)
		print_error(errno, "ftok error");	
	
	int registration_queue;
	if( (registration_queue = msgget(key, IPC_CREAT | 0600)) == -1)
	       print_error(errno, "msgget error");	

	struct message message;
	struct client *clients = malloc(sizeof(struct client) * MAX_CLIENTS_NUMBER);
	init_clients(clients);
	while(1){
		usleep(500000);
		if(msgrcv(registration_queue, &message, MAX_CLIENT_NAME_LENGTH, 0, IPC_NOWAIT) == -1){
			if(errno == E2BIG)
				print_error(errno, "client name too long");
			else if (errno != ENOMSG) // empty queue is ok
				print_error(errno, "msgrcv server_queue error");
		} else {
			int i;
			for(i = 0; i < MAX_CLIENTS_NUMBER; i++){
				if(clients[i].name[0] == '\0') break;
			}
			if(clients[i].name[0] != '\0')
				print_error(errno, "maximum number of clients already reached");
			else {
				key_t tmp_client_key;
				strcpy(clients[i].name, message.message);
				if( (tmp_client_key = ftok(CHAT_PATH, message.type)) == -1)
					print_error(errno, "ftok client error");
				if( (clients[i].queue = msgget(tmp_client_key, 0600)) == -1)
					print_error(errno, "msgget klient error");
				char *info1 = "------------  ";
				char *info2 =" has  joined the channel  ------------\n"; 
				char m[strlen(info1) + strlen(info2) + strlen(clients[i].name) + 1];
				strcpy(m, info1);
				strcat(m, clients[i].name);
				strcat(m, info2);
				send_message(m, i, clients);
			}
		}

		int i;
		for(i = 0; i < MAX_CLIENTS_NUMBER; i++){
			if(clients[i].name[0] != '\0'){
				if(msgrcv(clients[i].queue, &message, MAX_MESSAGE_SIZE, CLIENT_TO_SERVER, IPC_NOWAIT) == -1){
					
					int tmp_errno = errno;
					if(tmp_errno == EINVAL){
						char *info1 = "------------  ";
						char *info2 =" has  left the channel  ------------"; 
						char m[strlen(info1) + strlen(info2) + strlen(clients[i].name) + 1];
						strcpy(m, info1);
						strcat(m, clients[i].name);
						strcat(m, info2);
						clients[i].name[0] = '\0';
						send_message(m, i, clients);
					} else if (tmp_errno != ENOMSG)
						print_error(tmp_errno, "error in msgrcv in message reciver loop");
				} else if (strcmp(message.message, "exit\n") !=0){
					send_message(message.message, i, clients);
				}
			}

		}
	}
}

void init_clients(struct client *clients){
	int i;
	for(i = 0; i < MAX_CLIENTS_NUMBER; i++){
		clients[i].name[0] = '\0';	
	}
}

void send_message(char *m, int do_not_send_to, struct client *clients){
	struct message message;
	message.type = SERVER_TO_CLIENT;
	time_t t = time(NULL);
	char *string_time = ctime(&t);
	char message_to_send[strlen(m) + strlen(string_time) + 3];
	strcpy(message_to_send, "\n");
	strcat(message_to_send, string_time);
	strcat(message_to_send, " ");
	strcat(message_to_send, m);
	strcpy(message.message,message_to_send);
	strcat(message.message, "\0");
	printf("%s", message_to_send);
	int i;
	for(i = 0; i < MAX_CLIENTS_NUMBER; i++){
		if(i != do_not_send_to && clients[i].name[0] != '\0'){
			if( (msgsnd(clients[i].queue, &message, MAX_MESSAGE_SIZE, 0) == -1))
					print_error(errno, "error in send_message, msgsnd");
		}
		
	}
}
