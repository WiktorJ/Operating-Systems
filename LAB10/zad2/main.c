#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include "common.h"
#include <sys/select.h>
#include <unistd.h>
#include <bits/signum.h>
#include <signal.h>
#include <errno.h>
#define MAX_CLIENTS 10

int rc;
int sock;
int local_socket;
int conections_number=0;
struct server *servers;
fd_set fds[MAX_CLIENTS];
struct connection con[MAX_CLIENTS];


void print_error_exit(int error, char* message){
    puts(message);
    puts(strerror(error));
    exit(EXIT_FAILURE);
}


void print_error(int error, char* message){
    puts(message);
    puts(strerror(error));
}


void get_fd_set_ready(struct connection * arrConn, int nConn,
    struct server *arrServ, int nServ, int *fd_max){
    int m = -1;
    for(int i = 0;i<nServ;i++)
    {
        struct server *s = &arrServ[i];
        if(s->sock>=0)
        {
            if(m<s->sock)
            {
                m=s->sock;
            }
            FD_SET(s->sock,fds);
        }
    }

    for(int i = 0;i<nConn;i++)
    {
        struct connection *c = &arrConn[i];
        if(c->sock >=0)
        {
            if(m<c->sock)
            {
                m=c->sock;
            }
            FD_SET(c->sock,fds);
        }
    }
    *fd_max = m +1;
}

void new_connection_handler(struct server *pServer) {
    int sockTemp;

    struct connection conn;
    int len = sizeof(conn.addr);

    if( (sockTemp = accept(pServer->sock,(struct sockaddr *)&conn.addr,&len)) == -1)
		print_error(errno, "accept error1");
 

    len = sizeof(conn.local);
    if(getsockname(sockTemp,(struct sockaddr *)&conn.local,&len)==-1)
	print_error(errno, "getsockname error1");

    conn.sock=sockTemp;

    if(conections_number<MAX_CLIENTS)
    {
        for(int i = 0;i<MAX_CLIENTS;i++)
        {
            if(con[i].sock<0)
            {
                con[i] = conn;
                printf("New client connected\n");
                break;
            }
        }
        conections_number++;
    }
}

void existing_connection_handler(struct connection *c)
{
    char buf[MAX_MESSAGE_LENGTH];


    rc =(int) read(c->sock,buf,MAX_MESSAGE_LENGTH);
    if(rc == -1)
    {
        perror("read():");
        exit(EXIT_FAILURE);
    }
    if(rc==0)
    {
	FD_CLR(c->sock, fds);
	close(c->sock);
	c->sock = -1;
        return;
    }

    if(strncmp(buf,"#reg",3)==0)
    {
        strncpy(c->name,buf+3,MAX_NAME_LENGTH);
        int len = (int)strlen(c->name);
        printf("registered with name: %s\n",c->name);
        return;
    }
    buf[strlen(buf)-1]='\0';
    printf("%s:%s\n",c->name,buf);
    for(int i = 0;i<MAX_CLIENTS;i++)
    {
        if(con[i].sock==-1)
        {
            continue;
        }
        if(con[i].sock==c->sock)
        {
            continue;
        }
        char buff2[MAX_MESSAGE_LENGTH];
        memset(buff2,'\0',sizeof(buff2));

        strncpy(buff2,"\n",1);
        strncpy(buff2+1,c->name,strlen(c->name));
        strncpy(buff2+strlen(c->name)+1,":",strlen(buf));
        strncpy(buff2+strlen(c->name)+2,buf,strlen(buf));
        if( (rc=(int)write(con[i].sock,buff2,MAX_MESSAGE_LENGTH)) < 0)
        	print_error(errno, "write erro1");
    }
}



void atexitFun(void)
{
    system("rm -r socket1");
}

int main(int argc, char **argv) {

    servers=malloc(sizeof(struct server)*2);
    atexit(atexitFun);
    //clearing connection table
    for(int i = 0;i<MAX_CLIENTS;i++)
    {
        memset(&con[i],0,sizeof(struct connection));
        con[i].sock=-1;
    }


    sock= -1;
    int t = 1;
    int fd_max = -1;
    struct sockaddr_in structsockaddr_in;


    if( (sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1)
    	print_error(errno, "socket error1");

    if( (rc = setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&t,sizeof(t))) == -1)
	print_error(errno, "setsocketotp error2");    
    
   

    structsockaddr_in.sin_family=AF_INET;
    structsockaddr_in.sin_port=htons(1100);
    structsockaddr_in.sin_addr.s_addr=htonl(INADDR_ANY);
    servers[0].addr=structsockaddr_in;
    servers[0].sock=sock;
    servers[0].type=INTERNET_TYPE;

    if( (rc = bind(sock,(struct sockaddr *)&structsockaddr_in,sizeof(structsockaddr_in))) == -1)
		print_error(errno, "bind error1");
 
   
    if( (rc = listen(sock,1)) == -1)
		print_error(errno, "listen error1");    


    struct sockaddr_un sockaddr_un1;

    if( (local_socket = socket(AF_UNIX,SOCK_STREAM,0)) == -1)
		print_error(errno, "socket error2");
    sockaddr_un1.sun_family=AF_UNIX;
    strcpy(sockaddr_un1.sun_path,"socket1");
    int servlen = (int)strlen(sockaddr_un1.sun_path)+(int)sizeof(sockaddr_un1.sun_family);
    servers[1].type=LOCAL_TYPE;
    servers[1].addrLocal=sockaddr_un1;
    servers[1].sock=local_socket;
    if( (rc=bind(local_socket,(struct sockaddr *)&sockaddr_un1,servlen)) == -1)
	print_error(errno, "bind error2");
    
    if( (rc=listen(local_socket,5)) == -1)
	print_error(errno, "listen error2");
    

    printf("Server is up and running...\n");
    while(1)
    {
        get_fd_set_ready(con,MAX_CLIENTS,servers,2,&fd_max);

        if( (rc = select(fd_max,fds,NULL,NULL,NULL)) == -1)
        	print_error(errno, "slect error3");


        for(int i = 0;i<2;i++) {
            if ((servers[i].sock >= 0) && FD_ISSET(servers[i].sock, fds)) {
                new_connection_handler(&servers[i]);
            }
        }

        for(int i =0;i<MAX_CLIENTS;++i)
        {
            struct connection *c = &con[i];
            if(c->sock>=0 && FD_ISSET(c->sock,fds))
            {
                existing_connection_handler(c);
            }
        }
    }
    return EXIT_SUCCESS;
}

