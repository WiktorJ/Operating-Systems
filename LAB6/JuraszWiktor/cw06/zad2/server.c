#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#define MAX_CLIENTS_NUMBER 10
#define MAX_CLIENT_NAME_LENGTH 128
#define SERVER_PRIORITY 0
#define CLIENT_PRIORITY 1
void register_client();
void provide_message();
void init_clients();


struct client {
		mqd_t send_queue;
		mqd_t receive_queue;
		char name[MAX_CLIENT_NAME_LENGTH];
		struct sigevent sigev;
};

void print_error(int error, char* message){
	puts(message);
	puts(strerror(error));
}
struct client clients[MAX_CLIENTS_NUMBER];
mqd_t server_queue;
struct mq_attr attributes;
void interrupt_handler() {
	mq_close(server_queue);
	mq_unlink("/registration_queue");
	int i;
		for(i = 0; i <  MAX_CLIENTS_NUMBER ; i++) {
			mq_close(clients[i].send_queue);
		}
	exit(EXIT_SUCCESS);
}


int main(int argc, char **argv) {
	signal(SIGINT, interrupt_handler);
	init_clients();

	attributes.mq_flags = 0;
	attributes.mq_maxmsg = 50;
	attributes.mq_msgsize = 1024;
	if( (server_queue = mq_open("/registration_queue", O_RDONLY | O_CREAT, 0664, &attributes)) == -1)
			print_error(errno, "mq_open error");

	while(1){
		register_client();
		provide_message();
	}
	exit(EXIT_SUCCESS);
}

void register_client(){
	int i;
	for(i = 0; i < MAX_CLIENTS_NUMBER; i++){
		if(clients[i].name[0] == '\0') break;
	}
	if(clients[i].name[0] != '\0')
		print_error(errno, "maximum number of clients already reached");
	
		if(mq_receive(server_queue, clients[i].name, MAX_CLIENT_NAME_LENGTH, NULL) != -1){
		char client_send_queue_name[strlen("_send_queue") + strlen(clients[i].name) + 1];
		char client_receive_queue_name[strlen("_receive_queue") + strlen(clients[i].name) + 1];
		strcpy(client_send_queue_name, "/");
		strcat(client_send_queue_name, clients[i].name);
		strcat(client_send_queue_name, "_send_queue");	
		clients[i].send_queue = mq_open(client_receive_queue_name, O_RDWR);

		strcpy(client_receive_queue_name, "/");
		strcat(client_receive_queue_name, clients[i].name);
		strcat(client_receive_queue_name, "_receive_queue");	
		clients[i].receive_queue = mq_open(client_send_queue_name, O_RDWR);

		printf("--------- %s has joined the channel ---------------", clients[i].name);
		fflush(stdout);
		}
	


}

void provide_message(){
	
	int i;
	for(i = 0; i< MAX_CLIENTS_NUMBER; i++){	
		if(clients[i].name[0] !='\0'){
			char buff[attributes.mq_msgsize];
			if(mq_receive(clients[i].receive_queue, buff, attributes.mq_msgsize, NULL) == -1){
				char *message;	
				if(strcmp(buff, "exit\n") == 0){
					char *end_m = " has left the chat";
					message = malloc(sizeof(char) * (strlen(end_m) + strlen(clients[i].name)));
					strcpy(message, clients[i].name);
					strcat(message, end_m);
					clients[i].name[0] = '\0';
				} else{
					time_t t = time(NULL);
					char *time_string = ctime(&t);
					message = malloc(sizeof(char) * (strlen(buff) + strlen(time_string) + strlen(clients[i].name + 4)));
					strcpy(message, clients[i].name);
					strcat(message, ": ");
					strcat(message, time_string);
					strcat(message, ": ");
					strcat(message, buff);
				}
				int j;
				for(j = 0; j< MAX_CLIENTS_NUMBER; j++){
					if(j != i && clients[i].name[0] !='\0'){
						if(mq_send(clients[i].send_queue, message, strlen(message), 0) == -1)
							print_error(errno, "mq_send in sending messages to all error");
					}
				}
				
			}
	
		}
	}
}

void init_clients(){
    int i;
        for(i = 0; i < MAX_CLIENTS_NUMBER; i++){
                clients[i].name[0] = '\0';
        }
}
