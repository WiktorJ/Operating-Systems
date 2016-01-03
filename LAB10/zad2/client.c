#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdio.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "common.h"

int fd_socket;
int rc2;
struct connection conn;
fd_set fds;
void writer(void *ptr) ;
void receiver(void *ptr);
char *name;

void print_error_exit(int error, char* message){
    puts(message);
    puts(strerror(error));
    exit(EXIT_FAILURE);
}


void print_error(int error, char* message){
    puts(message);
    puts(strerror(error));
}

void atexit_function(void) {
	close(fd_socket);
}

int main(int argc, char **argv) {
    if(argc !=5) {
        printf("usage: ./client <type 1=internet, other = local> <id> <if 1 on type => ip else path> <if 1 on type => port else whateve>\n");
        exit(EXIT_FAILURE);
    }

    atexit(atexit_function);
    
    name = malloc(sizeof(char)*MAX_NAME_LENGTH);
    memset(name,'\0',sizeof(name));
    strcpy(name,argv[2]);

    if(atoi(argv[1])==1) {
        struct sockaddr_in stSockAddr;
        int res;
        fd_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        conn.sock = fd_socket;

        if (fd_socket == -1)
		print_error(errno, "socket error1");

        memset(&stSockAddr, 0, sizeof(stSockAddr));

        stSockAddr.sin_family = AF_INET;
        stSockAddr.sin_port = htons((uint16_t )atoi(argv[4]));
        res = inet_pton(AF_INET, argv[3], &stSockAddr.sin_addr);

        if (res <= 0 ) 
		print_error(errno, "ip addres is not valid");

        if (connect(fd_socket, (struct sockaddr *) &stSockAddr, sizeof(stSockAddr)) == -1 ) 
		print_error(errno, "connect error1");
    }

    if(atoi(argv[2])==0)
    {
        fd_socket=socket(AF_UNIX,SOCK_STREAM,0);
        conn.sock=fd_socket;
        struct sockaddr_un serv;
        memset(&serv,0,sizeof(serv));

        strcpy(serv.sun_path,argv[3]);
        serv.sun_family=AF_UNIX;
        int len = (int)strlen(serv.sun_path)+(int)sizeof(serv.sun_family);

        if( (rc2=connect(fd_socket,(struct sockaddr *)&serv,len)) == -1);
        	print_error(errno, "connect error2");

    }

    pthread_t senderThread;
    pthread_t receiverThread;
    pthread_create(&senderThread,NULL,(void*)writer,(void*)(intptr_t)fd_socket);
    pthread_create(&receiverThread,NULL,(void*)receiver,(void*)(intptr_t)fd_socket);
    pthread_join(receiverThread,NULL);
    pthread_join(senderThread,NULL);

    (void) shutdown(fd_socket, SHUT_RDWR);
    close(fd_socket);
    printf("Client terminated!\n");
    return EXIT_SUCCESS;
}

void writer(void *ptr) {

    char buff2[MAX_MESSAGE_LENGTH]={0};
    memset(buff2,'\0',sizeof(buff2));
    char *nickNotification="#reg";
    int sock = (intptr_t)ptr;


    strcpy(buff2,nickNotification);
    strcpy(buff2+3,name);
    write(sock, buff2,MAX_MESSAGE_LENGTH);

    while (fgets(buff2, sizeof(buff2), stdin)!=NULL) {
        int rec = (int)write(sock, buff2, MAX_MESSAGE_LENGTH);
        if (rec == -1) 
		print_error(errno, "write error1");
    }

    printf("Sender exited \n");
    pthread_exit((void *) EXIT_SUCCESS);
}

void receiver(void *ptr) {
    char buff[MAX_MESSAGE_LENGTH] ={0};
    int sock = (intptr_t)ptr;
    FD_ZERO(&fds);
    FD_SET(sock,&fds);
    while (1) {
        select(sock+1, &fds, NULL, NULL, NULL);
        if(FD_ISSET(sock,&fds))
        {
            int num = (int) read(sock, buff, MAX_MESSAGE_LENGTH);
            if (num > 0) {
                printf("Received message:%s\n", buff);
            }
        }
    }


}
