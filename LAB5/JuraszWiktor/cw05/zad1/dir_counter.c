#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>


void print_error(int error, char* message){
	puts(message);
	puts(strerror(error));
}

int main(int argc, char* argv[]){

	int fd[2];
	pid_t pid;
	
	if(argc !=1){
		printf("usage: ./dir_counter");
		exit(EXIT_FAILURE);
	}
	if (pipe(fd) < 0)
		print_error(errno,"pipe error");

	if( (pid = fork()) < 0)
		print_error(errno, "fork error");
	else if(pid > 0) {
		close(fd[0]); //closing read end
		if(fd[1] != STDOUT_FILENO){
			if(dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO)
				print_error(errno, "dup2 error to stdout");
			close(fd[1]);	
		}
		if(execl("/bin/ls", "ls", "-l", (char*)0) < 0) 
			print_error(errno, "execl error");

	} else {
		close(fd[1]);
		if(fd[0] != STDIN_FILENO){
			if(dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
				print_error(errno, "dup2 errno in stdin");
			close(fd[0]);
			}
		pid_t pid2;
      		int fd2[2];
		
		if (pipe(fd2) < 0)
			print_error(errno,"pipe error");

		if( (pid2 = fork()) < 0)
			print_error(errno, "fork error");
		else if(pid2 > 0) {
			close(fd2[0]); //closing read end
				if(fd2[1] != STDOUT_FILENO){
					if(dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
						print_error(errno, "dup2 error to stdout");
				close(fd2[1]);	
			}
			if(execl("/bin/grep", "grep", "^d", (char*)0) < 0) 
				print_error(errno, "execl error grep ^d");


		} else {
			close(fd2[1]);
			if(fd2[0] != STDIN_FILENO){
				if(dup2(fd2[0], STDIN_FILENO) != STDIN_FILENO)
					print_error(errno, "dup2 errno in stdin second fork");
				//close(fd2[0]);
				}
		
			
			if(execl("/usr/bin/wc", "wc", "-l", (char*)0) < 0)
				print_error(errno, "execl error wc -l");
	
		}
	}
return 0;
}
