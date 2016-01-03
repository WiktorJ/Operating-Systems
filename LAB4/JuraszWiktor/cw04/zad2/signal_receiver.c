#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/resource.h>



int sigusr1_counter;
int sigusr2_recived;
void sigusr1_handler(int signo){
	sigusr1_counter++;
}

void sigusr2_handler(int signo){
	sigusr2_recived = 1;
}


void print_error(int error, char *message){
	puts(message);
	puts(strerror(error));
}


int main(int argc, char *argv[]){
	struct sigaction *sigusr1_sigaction = malloc(sizeof(struct sigaction));
	struct sigaction *sigusr2_sigaction = malloc(sizeof(struct sigaction));

	sigusr1_sigaction -> sa_handler = sigusr1_handler;
	sigusr2_sigaction -> sa_handler = sigusr2_handler;

	if(sigfillset(&(sigusr1_sigaction -> sa_mask)) == -1) print_error(errno, "error in sigfillset(sigusr1)");
	if(sigfillset(&(sigusr2_sigaction -> sa_mask)) == -1) print_error(errno, "error in sigfillset(sigusr2)");

	if(sigaction(SIGUSR1, sigusr1_sigaction, NULL) == -1) print_error(errno, "errno in sigaction(SIGUER1)");
	if(sigaction(SIGUSR2, sigusr2_sigaction, NULL) == -1) print_error(errno, "errno in sigaction(SIGUER2)");

	sigset_t ss, oldmask;
	pid_t pid = getppid();
	sigusr2_recived = 0;
	sigusr1_counter = 0;

	if(sigaddset(&ss, SIGUSR1) == -1) print_error(errno, "error in sigaddset 1");
	if(sigaddset(&ss, SIGUSR2) == -1) print_error(errno, "error in sigaddset 2");

	int i;
	sigemptyset(&oldmask);	
	while(!sigusr2_recived){
		sigsuspend(&oldmask);		
		if(kill(pid, SIGUSR1) != 0) print_error(errno, "error in SIGUSR1 kill");
	}

	if(kill(pid, SIGUSR2) != 0) print_error(errno, "error in SIGUSR2 kill");


	printf("\nCHILD: sent: %d\n", sigusr1_counter); 
	fflush(stdout);
	free(sigusr1_sigaction);	
	free(sigusr2_sigaction);	

	return 0;

	}



