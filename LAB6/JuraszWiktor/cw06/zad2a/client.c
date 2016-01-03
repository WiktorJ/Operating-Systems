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
mqd_t client_queue;
char* queue_name;

void print_error(int error, char* message){
	        puts(message);
		        puts(strerror(error));
}


int main(int argc, char **argv) {

	if(argc != 2){
		printf("usage: ./client <queue_name>");
		exit(EXIT_FAILURE);
    }
    printf("\nDo not use '#' character in messages, since is used in client-server communication\n"); 
	char queue_name[strlen("_queue") + strlen(argv[1]) + 1];
    strcpy(queue_name, "/");
	strcat(queue_name, argv[1]);
	strcat(queue_name, "_queue");

	struct mq_attr attribute;
	attribute.mq_flags = O_NONBLOCK;
	attribute.mq_maxmsg = 50;
	attribute.mq_msgsize = 1024;			
	if( (client_queue = mq_open(queue_name, O_RDWR | O_CREAT | O_NONBLOCK, S_IRUSR | S_IWUSR, &attribute)) == -1)
		print_error(errno, "error mq_open");
	
    if( (server_queue = mq_open("/registration_queue", O_WRONLY)) == -1)
		print_error(errno, "mq open server, error");	
	
	
    char message[attribute.mq_msgsize];
    sprintf(message,"register#%s", argv[1]);
    if(mq_send(server_queue, message, strlen(message), 0) == -1)	
		print_error(errno, "mq_send error during registration");

	printf("Connected\n");

	pid_t writing_pid = getpid();
	pid_t reading_pid = fork();

	if(reading_pid == 0){
		char buff[attribute.mq_msgsize];
		while(kill(writing_pid, 0) == 0){		
			usleep(500000);
			if(mq_receive(client_queue, buff, attribute.mq_msgsize, 0) == -1){
				int tmp_errno = errno;
				if(tmp_errno != EAGAIN)
					print_error(tmp_errno, "mq_receiver in reading procces");	
            } else
                 printf("%s", buff);
		}
		mq_unlink(queue_name);
		exit(EXIT_SUCCESS);
	}

    
char buff[attribute.mq_msgsize];
	while(1){
		fgets(buff, attribute.mq_msgsize, stdin);
		sprintf(message,"%s#%s", argv[1], buff);

        if(mq_send(server_queue, message, strlen(message), 0) == -1)
	        print_error(errno, "error in msgsend");
	        
		if(strcmp(buff, "exit\n") == 0)
	        break;	
	}

	exit(EXIT_SUCCESS);
}	
