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
void handle_message(char *name);
void init_clients();
void tstp_handler(int signo);
void atexit_function();
int register_client(char * name);
struct client {
		mqd_t queue;
		char name[MAX_CLIENT_NAME_LENGTH];
        int active;
};

void print_error(int error, char* message){
	puts(message);
	puts(strerror(error));
}
struct client clients[MAX_CLIENTS_NUMBER];
int clients_counter = 0;
mqd_t server_queue;
struct mq_attr attributes;

int main(int argc, char **argv) {
	

	atexit(atexit_function);
    
    signal(SIGTSTP, tstp_handler);     
   init_clients(); 
    attributes.mq_flags = O_NONBLOCK;
	attributes.mq_maxmsg = 50;
	attributes.mq_msgsize = 1024;
	if( (server_queue = mq_open("/registration_queue", O_RDONLY | O_CREAT | O_NONBLOCK, S_IRUSR | S_IWUSR, &attributes)) == -1){
			print_error(errno, "mq_open error");
            exit(EXIT_FAILURE);
}

    char message[attributes.mq_msgsize];
	while(1){
    usleep(50000);
    
    memset(&message[0], 0, sizeof(message));
    if(mq_receive(server_queue, message , attributes.mq_msgsize , 0) != -1){
            handle_message(message);
    }
    }
	
        
    mq_unlink("/registration_queue");
	exit(EXIT_SUCCESS);
}

void handle_message(char *message){
	
    char *n = (char*)memchr(message,'#',strlen(message));
    char type[1024];
    char message_to_send[1024];
    char name[1024];
    int a = n - message;
    strncpy(name, message + a + 1, strlen(message) - a + 1);
    strncpy(type,message,a);
    type[a] = '\0';
	if(strcmp(type,"register") == 0){
           if(register_client(name) == -1)
               printf("\ncoulnt registered client\n");
           else
               printf("\nclient %s registered successfuly\n", name);
        }
    else {
        int i;
        time_t t;
        t = time(NULL);
        sprintf(message_to_send,"%s: %s", ctime(&t), message);
        for(i = 0; i < clients_counter; i++){
            if((clients[i].active == 1) && (strcmp(clients[i].name, type) != 0)){
                if(mq_send(clients[i].queue, message_to_send, strlen(message_to_send), 0) == -1)
                    print_error(errno, "error in mq_send");
                else
                    printf("message sent to: %s\n", clients[i].name);
            }
        }
    
    }


}

int register_client(char *name){
    printf("\n name: %s",name);
    if(clients_counter == MAX_CLIENTS_NUMBER - 1)
        return -1;

    strcpy(clients[clients_counter].name, name);
    
    char queue_name[1024];
    sprintf(queue_name,"/%s_queue",name);
    if( (clients[clients_counter].queue = mq_open(queue_name, O_WRONLY | O_NONBLOCK)) == -1)
        print_error(errno, "mq_open error in client queue registration");

    clients[clients_counter].active = 1;
    clients_counter++;
    return 0;
}

void init_clients(){
   int  i;
   for(i = 0; i<MAX_CLIENTS_NUMBER; i++)
       clients[i].active = 0;
}



void tstp_handler(int signo){
        printf("in hander + %d", signo);
        fflush(stdout);
		exit(EXIT_SUCCESS);
}

void atexit_function(){
		mq_unlink("/registration_queue");
}
