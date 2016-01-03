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
	if(argc != 2){
		printf("usage: ./client <name>");
		exit(EXIT_FAILURE);
	}

	if(strlen(argv[1]) > MAX_CLIENT_NAME_LENGTH){
		printf("\nclient name to long, max lenght is: %d", MAX_CLIENT_NAME_LENGTH);
		exit(EXIT_FAILURE);
	}

	key_t client_key;
	pid_t pid = getpid();
       	if( (client_key = ftok(CHAT_PATH, pid)) == -1)
		print_error(errno, "ftok client_key error");
	
	int client_queue;
	if( (client_queue = msgget(client_key, IPC_CREAT | IPC_EXCL | 0600)) == -1)
		print_error(errno, "msgget client_queue error");
	
	
	key_t server_key;
       	if( (server_key = ftok(CHAT_PATH, CHAT_KEY)) == -1)
		print_error(errno, "ftok server_key error");
	
	int server_queue;
	if( (server_queue = msgget(server_key, 0200)) == -1)
		print_error(errno, "msgget server_queue error");

	struct message message;
	message.type = pid;
	strcpy(message.message, argv[1]);

	if(msgsnd(server_queue, &message, MAX_CLIENT_NAME_LENGTH, IPC_NOWAIT) == -1)
		print_error(errno, "msgsnd error, registering client");

	pid_t writing_pid = pid;
	pid_t reading_pid = fork();

	if(reading_pid == 0){
		while(kill(writing_pid, 0) == 0){
			usleep(500000);
			if(msgrcv(client_queue, &message, MAX_MESSAGE_SIZE, SERVER_TO_CLIENT, IPC_NOWAIT) == -1){
				int tmp_errno = errno;
				if(tmp_errno != ENOMSG)
					print_error(errno, "msgrcv errno clien_key");
			} else {
				printf("%s", message.message);

			}
		}
		if(msgctl(client_queue, IPC_RMID, (struct msqid_ds *) NULL) == -1)
			print_error(errno, "msgctl error");	
		printf("closin chat...");
		exit(EXIT_SUCCESS);
	}

	message.type = CLIENT_TO_SERVER;
	while(1){
		fgets(message.message, MAX_MESSAGE_SIZE, stdin);
		if(msgsnd(client_queue, &message, MAX_MESSAGE_SIZE, 0) == -1)
			print_error(errno, "error in msgsend");
		if(strcmp(message.message, "exit\n") == 0){
			break;
		}
	} 

	exit(EXIT_SUCCESS);
}
