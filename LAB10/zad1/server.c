#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#define SOCKETNAME_SERVER "SktOne"
#define SOCKETNAME_CLIENT "SktTwo"
#define MSG_SIZE 256

static struct sockaddr_un sa_server;

void print_error_exit(int error, char* message){
    puts(message);
    puts(strerror(error));
    exit(EXIT_FAILURE);
}


void print_error(int error, char* message){
    puts(message);
    puts(strerror(error));
}

int  main(int argc, char **argv){


    strcpy(sa_server.sun_path, SOCKETNAME_SERVER);
    sa_server.sun_family = AF_UNIX;
    (void)unlink(SOCKETNAME_SERVER);
    int fd_skt;
    ssize_t nrecv;
    char msg[MSG_SIZE];
    struct sockaddr_storage sa;
    socklen_t sa_len;
    if( (fd_skt = socket(AF_UNIX, SOCK_DGRAM, 0) ) == -1)
        print_error(errno, "scoker error");
    if( (bind(fd_skt, (struct sockaddr *)&sa_server, sizeof(struct sockaddr_un))) == -1 )
        print_error(errno, "bind error");
    while (1) {
         sa_len = sizeof(sa);
         if( (nrecv = recvfrom(fd_skt, msg, sizeof(msg), 0, (struct sockaddr *)&sa, &sa_len) ) == -1)
            print_error(errno, "recvfrom error");
         if (nrecv != sizeof(msg)) {
             printf("server got short message\n");
             break;
         }
         msg[0] = 'm';
         if(sendto(fd_skt, msg, sizeof(msg), 0, (struct sockaddr *)&sa, sa_len) == -1 )
            print_error(errno, "sendto error");
    }
    if( close(fd_skt) == -1)
        exit(EXIT_FAILURE);
    exit(EXIT_SUCCESS);
}


