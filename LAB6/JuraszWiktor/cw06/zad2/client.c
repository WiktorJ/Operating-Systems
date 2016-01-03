#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#define SERVER_PRIORITY 0
#define CLIENT_PRIORITY 1

mqd_t server_queue;
mqd_t client_sender_queue;
mqd_t client_receive_queue;
char* queue_name;

void print_error(int error, char* message){
	        puts(message);
		        puts(strerror(error));
}

struct mq_attr attribute;
int main(int argc, char **argv) {

	if(argc != 2){
		printf("usage: ./client <queue_name>");
		exit(EXIT_FAILURE);
	}
 	
	 char client_send_queue_name[strlen("_send_queue") + strlen(argv[1]) + 50];
       //  strcpy(client_send_queue_name, "/");
	// strcat(client_send_queue_name, argv[1]);
	// strcat(client_send_queue_name, "_send_queue");  
	sprintf(client_send_queue_name,"/%s_send_queue",argv[1]);

	char client_receive_queue_name[strlen("_receive_queue") + strlen(argv[1]) + 1];
        strcpy(client_receive_queue_name, "/");
	strcat(client_receive_queue_name, argv[1]);
	strcat(client_receive_queue_name, "_receive_queue");    

	printf("%s", client_send_queue_name);
	printf("%s",argv[1]);
	fflush(stdout);
	attribute.mq_flags = O_NONBLOCK;
	attribute.mq_maxmsg = 50;
	attribute.mq_msgsize = 1024;			
	if( (server_queue = mq_open("/registration_queue", O_WRONLY, 0664, &attribute)) == -1)
		print_error(errno, "mq open server, error");	
	
	if(mq_send(server_queue, argv[1], strlen(argv[1]), 0) == -1)	
		print_error(errno, "mq_send to server_queue error");

	if( (client_sender_queue = mq_open(client_send_queue_name, O_RDWR | O_CREAT, 0664, &attribute)) == -1)
		print_error(errno, "error mq_open2");
	if( (client_receive_queue = mq_open(client_receive_queue_name, O_RDWR | O_CREAT, 0664, &attribute)) == -1)
		print_error(errno, "error mq_open1");

	


	pid_t writing_pid = getpid();
	pid_t reading_pid = fork();

	if(reading_pid == 0){
		char buff[attribute.mq_msgsize];
		while(kill(writing_pid, 0) == 0){		
			usleep(500000);
			if(mq_receive(client_receive_queue, buff, attribute.mq_msgsize, NULL) != -1){
				printf("%s", buff);
				fflush(stdout);
			}

		}
		mq_unlink(queue_name);
		exit(EXIT_SUCCESS);
	}

	char buff[attribute.mq_msgsize];
	while(1){
		fgets(buff, attribute.mq_msgsize, stdin);
		if(mq_send(client_sender_queue, buff, strlen(buff), 0) == -1)
	        	print_error(errno, "error in msgsend");	        
		if(strcmp(buff, "exit\n") == 0)
	        	break;	
	}

	exit(EXIT_SUCCESS);
}
