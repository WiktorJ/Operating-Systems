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
		printf("usage: ./rdfifo <queue_name>");
		exit(EXIT_FAILURE);
	}

	printf("to close server send 'close_server' message from client\n");
	fflush(stdout);

	if( mkfifo(argv[1], S_IRUSR | S_IWUSR | S_IXUSR) < 0)
		       print_error(errno, "error in mkfifo");
	
	int fd;
	char buf[BUFFSIZE];
	fd = open(argv[1], O_RDWR);
	time_t read_time;
	
	while(1){
		int read_value;
		//if( (read_value = read(fd, buf, BUFFSIZE)) == 0){
			//close(fd);
			//if( mkfifo(argv[1], S_IRUSR | S_IWUSR | S_IXUSR) < 0)
		       	//	print_error(errno, "error in mkfifo in while");

		//	if( (fd = open(argv[1], O_RDONLY)) < 0)
		//		print_error(errno, "open in while error"); 
		//} else {
		
			if( (read_value = read(fd, buf, BUFFSIZE))< 0) 
				print_error(errno, "read error");
		
			time(&read_time);
			if(strcmp("close_server\n", buf) == 0){
				printf("Shutting down...");
				break;
			}

			char output[BUFFSIZE];
			strcpy(output, ctime(&read_time));
			strcat(output, "-");
			strcat(output, buf);
			printf("%s", output);

			memset(buf, 0, sizeof(buf));	
	
		//}

	}
	close(fd);
	unlink(argv[1]);

	return 0;
}
