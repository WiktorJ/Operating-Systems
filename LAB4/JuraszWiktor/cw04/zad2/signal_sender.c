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

void check_mask(sigset_t *ss){
	printf("\n------in check_mask------\n");
	if(sigismember(ss, SIGUSR1))printf("\nSIGUSR_1 ");
	if(sigismember(ss, SIGUSR2))printf("SIGUSR_2 ");
	if(sigismember(ss, SIGINT))printf("SIGINT\n");
	fflush(stdout);
}

int main(int argc, char *argv[]){
	if(argc != 2){
		printf("given %d arguments\n",argc);
		print_error(errno, "program takes one argument <number_of_iterations>");
		exit(EXIT_FAILURE);	
	}

	struct sigaction *sigusr1_sigaction = malloc(sizeof(struct sigaction));
	struct sigaction *sigusr2_sigaction = malloc(sizeof(struct sigaction));
	
	sigusr1_sigaction -> sa_handler = sigusr1_handler;
	sigusr2_sigaction -> sa_handler = sigusr2_handler;
	
	if(sigfillset(&(sigusr1_sigaction -> sa_mask)) == -1) print_error(errno, "error in sigfillset(sigusr1)");
	if(sigfillset(&(sigusr2_sigaction -> sa_mask)) == -1) print_error(errno, "error in sigfillset(sigusr2)");

	sigset_t ss;
	sigset_t oldmask;
	if(sigaddset(&ss, SIGUSR1) == -1) print_error(errno, "error in sigaddset 1");
	if(sigaddset(&ss, SIGUSR2) == -1) print_error(errno, "error in sigaddset 2");

	if(sigaction(SIGUSR1, sigusr1_sigaction, NULL) == -1) print_error(errno, "errno in sigaction(SIGUER1)");
	if(sigaction(SIGUSR2, sigusr2_sigaction, NULL) == -1) print_error(errno, "errno in sigaction(SIGUER2)");

	pid_t pid;
	sigusr2_recived = 0;	
	
	if(sigprocmask(SIG_BLOCK, &ss, NULL) == -1) print_error(errno, "errnr in sigprockmask SIG_BLOCK");
	
	if( (pid = fork()) == -1) print_error(errno, "error in fork()");
	if(pid == 0) execl("./signal_receiver", "signal_receiver", NULL);
		
	if(sigprocmask(SIG_UNBLOCK, &ss, NULL) == -1) print_error(errno, "errnr in sigprockmask SIG_UNBLOCK");
	int i;
	int process_number = atoi(argv[1]);
	if(sigprocmask(SIG_BLOCK, &ss, NULL) == -1) print_error(errno, "errnr in sigprockmask SIG_BLOCK");
	 sigemptyset(&oldmask);
	for(i = 0; i< process_number; i++){
		if(kill(pid, SIGUSR1) != 0) print_error(errno, "error in SIGUSR1 kill");
		sigsuspend(&oldmask);
	}

	
	if(kill(pid, SIGUSR2) != 0) print_error(errno, "error in SIGUSR2 kill");
	if(sigprocmask(SIG_UNBLOCK, &ss, NULL) == -1) print_error(errno, "errnr in sigprockmask SIG_UNBLOCK");
	while(!sigusr2_recived) pause();
	if(sigusr1_counter == process_number) printf("\n signals received equals to signale sent\n");
	else printf("\nsent: %d, received: %d\n", process_number, sigusr1_counter); 

	free(sigusr1_sigaction);	
	free(sigusr2_sigaction);	

	return 0;

	}



