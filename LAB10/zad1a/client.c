#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include <signal.h>
#include "common.h"
#include <errno.h>

int server_s;
int client_s;
struct sockaddr_in server;
struct sockaddr_un local_server;
fd_set fds;

void print_error_exit(int error, char* message){
    puts(message);
    puts(strerror(error));
    exit(EXIT_FAILURE);
}


void print_error(int error, char* message){
    puts(message);
    puts(strerror(error));
}


void receiver(void *ptr);

void writer(void *ptr);

int type;
char *name;

void atexit_function(void)
{
       close(client_s);
       (void)unlink(local_server.sun_path);
}
int main(int argc, char *argv[]) {

    if(argc != 5){
        printf("usage: ./client <type 1=internet, other = local> <id> <if 1 on type => ip else path> <if 1 on type => port else whateve>");
        exit(EXIT_FAILURE); 
    }

    atexit(atexit_function);
    int len = sizeof(struct sockaddr_in);
    char buf[MAX_MESSAGE_LENGTH];
    struct hostent *host;
    int n, port;


    type = atoi(argv[1]);
    name = argv[2];

    if (type == 1) {
        host = gethostbyname(argv[3]);
        if (host == NULL) 
		print_error(errno, "gethostbyname error1");
        port = atoi(argv[4]);


        if ((server_s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) 
            print_error(errno, "socket error1");

        memset((char *) &server, 0, sizeof(struct sockaddr_in));
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        server.sin_addr = *((struct in_addr *) host->h_addr_list[0]);
    }
    else {
        if( (server_s = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
            print_error(errno, "socket error2");
        
        strcpy(local_server.sun_path, argv[3]);
        local_server.sun_family = AF_UNIX;

        if( (client_s = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
            print_error(errno, "client_s error");

        struct sockaddr_un selfServer;
        strcpy(selfServer.sun_path, name);
        selfServer.sun_family = AF_UNIX;
        if(bind(client_s, (struct sockaddr *) &selfServer, sizeof(struct sockaddr_un)) == -1)
            print_error(errno, "socket error3");

    }

    pthread_t receiverThread;
    pthread_t writerThread;

    pthread_create(&receiverThread, NULL, (void *) receiver, NULL);
    pthread_create(&writerThread, NULL, (void *) writer, NULL);
    pthread_join(receiverThread, NULL);
    pthread_join(writerThread, NULL);

    close(server_s);
    return 0;
}

void writer(void *ptr) {
    struct msg msg1;
    memset(msg1.buf, '\0', sizeof(msg1.buf));
    memset(msg1.name, '\0', sizeof(msg1.name));
    strcpy(msg1.name, name);
    msg1.sock = client_s;
    while (1) {
        fgets(msg1.buf, 256, stdin);
        if (type == 1) {
            if (sendto(server_s, (void *) &msg1, sizeof(struct msg), 0, (struct sockaddr *) &server, sizeof(struct sockaddr_in)) == -1) 
            print_error(errno, "sendto error1");
        }
        else {
            if (sendto(server_s, (void *) &msg1, sizeof(struct msg), 0, (struct sockaddr *) &local_server, sizeof(struct sockaddr_un)) == -1) 
                print_error(errno, "sendto error2");
        }
    }

    pthread_exit((void *) EXIT_SUCCESS);
}

void receiver(void *ptr) {
    int n;
    struct msg msg1;
    memset(msg1.buf, '\0', sizeof(msg1.buf));
    memset(msg1.name, '\0', sizeof(msg1.name));
    int len = sizeof(struct sockaddr_in);

    FD_ZERO(&fds);
    if (type == 1) {
        FD_SET(server_s, &fds);
        while (1) {
            select(server_s + 1, &fds, NULL, NULL, NULL);
            recvfrom(server_s, (void *) &msg1, sizeof(struct msg), 0, NULL, 0);
            if(strcmp(msg1.name,name)==0)
            {
                continue;
            }
            printf("Message from:%s : %s\n", msg1.name, msg1.buf);
        }
    }
    else {
        FD_SET(client_s, &fds);
        while (1) {
            select(client_s + 1, &fds, NULL, NULL, NULL);
            recvfrom(client_s, (void *) &msg1, sizeof(struct msg), 0, NULL, 0);
            if(strcmp(msg1.name,name)==0)
            {
                continue;
            }
            printf("Message from:%s : %s\n", msg1.name, msg1.buf);
        }
    }

    pthread_exit((void *) EXIT_SUCCESS);
}
