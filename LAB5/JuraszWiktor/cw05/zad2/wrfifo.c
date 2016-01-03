#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

void print_error(int error, char* message){
	puts(message);
	puts(strerror(error));
}

const int BUFFSIZE = 1000;

int main(int argc, char* argv[]){
	
	if(argc != 2){
		printf("usage: ./wrfifo <queue_name>");
		exit(EXIT_FAILURE);
	}
	
	printf("to close client type: 'close_client'\n");
	printf("to close server type: 'close_server', note that it will also close client\n");
	
	int fd;
	char buf[BUFFSIZE];
	fd = open(argv[1], O_WRONLY);
	time_t write_time;
	
	while(1){
		fgets(buf, BUFFSIZE, stdin);
		if(strcmp(buf, "close_server\n") == 0){
			if (write(fd, buf, BUFFSIZE) < 0) 
				print_error(errno, "write error");
			}
		if(strcmp(buf, "close_client\n") == 0){
			break;
		}
		

		pid_t pid = getpid();
		char str_pid[10];
		sprintf(str_pid, "%d", pid);
		time(&write_time);
		char *str_time = ctime(&write_time);
		int message_len = strlen(str_pid) + strlen(str_time) + strlen(buf) + 2;
		char message[message_len];
		strcpy(message, str_pid);
		strcat(message, "-");
		strcat(message, str_time);
		strcat(message, "-");
		strcat(message, buf);

		if(write(fd, message, message_len) < 0)
			print_error(errno, "write error 2");

		memset(buf, 0, sizeof(buf));	
	}

	close(fd);

	return 0;
}
