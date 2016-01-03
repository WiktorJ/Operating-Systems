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

const int BUFFSIZE = 10000;

int main(int argc, char* argv[]){
	FILE *input, *output;
		if( (output = fopen("folders.txt", "w")) == NULL)
			print_error(errno, "error in fopen, fpin is NULL");
		if( (input = popen("ls -l | grep ^d", "r")) == NULL)
			print_error(errno, "errno in popen");

	char buff[BUFFSIZE];
		while(fgets(buff, BUFFSIZE, input) != NULL){
			      if(fputs(buff, output) == EOF)
				     print_error(errno, "fput error");
			}
		if(fclose(output) == -1)
			print_error(errno, "fclose error");
		
		if(pclose(input) == -1)
			print_error(errno, "pclose error");
return 0;
}		

