#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/syscall.h>
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


//void signal_handler(int signal_number, siginfo_t *info, void *ptr){
  //  pid_t tid;
   // tid = syscall(SYS_gettid);
   // printf("Signal %d  TiD: %ld \n", signal_number, (long) tid);
//}

//void alarm_handler(int signal_number, siginfo_t *info, void *ptr){
  //  pid_t tid;
   // tid = syscall(SYS_gettid);
   // printf("Signal %d  TiD: %ld \n", signal_number, (long) tid);
//}
int fd_skt;
int self_socket;
char *name;
void atexit_function(void){
    close(fd_skt);
}

pthread_t rdwr_thread;

//struct sigaction sa;
//struct sigaction sa2;
//struct sigaction sa3;


void* communication_thread(void *arg){
    int i;
    
    for (i = 1; i <= 4; i++) {
        snprintf(msg, sizeof(msg), "Message #%d", i);
        if( sendto(fd_skt, msg, sizeof(msg), 0, (struct sockaddr *)&sa_server, sizeof(sa_server)) == -1)
            print_error(errno, "sendto error");
        if( (nrecv = read(fd_skt, msg, sizeof(msg))) == -1)
            print_error(errno, "read error");
        if (nrecv != sizeof(msg)) {
            printf("client got short message\n");
            break;
        }
        printf("Got \"%s\" back\n", msg);
    }

}

void* write_thread(void *arg){

    //sigset_t ss;
    //sa3.sa_sigaction = alarm_handler;
    //if(sigaddset(&ss, SIGUSR1) == -1)
     //   print_error(errno, "sigaddset error");

    //if(pthread_sigmask(SIG_SETMASK, &ss, NULL) == -1)
      //      print_error(errno, "sigprocmask");

    //if(sigaction(SIGUSR2, &sa3, NULL) == -1)
      //  print_error(errno, "sigaction thread error");
    
    

}

int  main(int argc, char **argv){
    if(argc != 4){
        printf("usage: ./client <type 1=internet, other = local> <id> <if 1 on type => ip else path> <if 1 on type => port else whateve>");
        exit(EXIT_FAILURE); 
    }


    atexit(atexit_function);
    int len = sizeof(struct sockaddr_in);
    struct hostent *host;
    int n, port;

    int type = atoi(argv[1]);
    name = argv[2];
    
    
    
    
  


    //sa.sa_sigaction = signal_handler;
    //sa2.sa_sigaction = alarm_handler;
    //if(sigaction(SIGUSR1, &sa, NULL) == -1)
      //  print_error(errno, "sigaction thread error");
    
   // if(sigaction(SIGALRM, &sa2, NULL) == -1)
     //   print_error(errno,"sigaction error");
       
    //sigset_t ss;
    //if(sigaddset(&ss, SIGUSR2) == -1)
      //  print_error(errno, "sigaddset error");

    //if(pthread_sigmask(SIG_SETMASK, &ss, NULL) == -1)
      //      print_error(errno, "sigprocmask");
    
    ssize_t nrecv;
    char msg[MSG_SIZE];
    int i;
    if(type == 1) {
        printf("xD");
    } else {
        if( (fd_skt = socket(AF_UNIX, SOCK_DGRAM, 0) ) == -1)
            print_error(errno, "scoket error");m
    
        strcpy(sa_server.sun_path, argv[3]);
        sa_server.sun_family = AF_UNIX;
    
        snprintf(sa_client.sun_path, sizeof(sa_client.sun_path), "%s-%d", name , 1);
        (void)unlink(sa_client.sun_path);
        sa_client.sun_family = AF_UNIX;
        struct sockaddr_un sa_client;
    
        if(bind(self_socket, (struct sockaddr *)&sa_client, sizeof(sa_client)) == -1 )
            print_error(errno, "bind error");

    }
   if( close(fd_skt) )
    exit(EXIT_SUCCESS);
   exit(EXIT_FAILURE);
}
