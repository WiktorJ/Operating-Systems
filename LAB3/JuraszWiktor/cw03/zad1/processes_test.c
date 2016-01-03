#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/resource.h>
int counter = 0;
//suseconds_t partial_real_time;
const int STACK_SIZE = 50000;
#define MACRO_SECONDS 1000000
#define CLOCK 100
void print_error(int error, char *message){
	puts(message);
	puts(strerror(error));
}



void time_checker(struct tms *exe_times, suseconds_t c_real_time, clock_t p_real_time ,FILE *parent_handle, FILE * child_handle, FILE *sum_handle){
	clock_t  p_prev_user = exe_times -> tms_utime;
	clock_t  p_prev_sys = exe_times -> tms_stime;	
	clock_t  c_prev_user = exe_times -> tms_cutime;	
	clock_t  c_prev_sys = exe_times -> tms_cstime;	
	p_real_time = times(exe_times) - p_real_time;
	
	clock_t p_sys_time = exe_times -> tms_stime - p_prev_sys;
	clock_t p_user_time = exe_times -> tms_utime - p_prev_user;
	clock_t c_sys_time = exe_times -> tms_cstime - c_prev_sys;
	clock_t c_user_time = exe_times -> tms_cutime - c_prev_user;
	if(fprintf(parent_handle,"%10.2lf %10.2lf %10.2lf %10.2lf", (double) p_sys_time / CLOCK, (double) p_user_time / CLOCK, (double) (p_sys_time + p_user_time) / CLOCK ,(double) p_real_time/CLOCK) < 0) perror("time_checker fprintf error parent_handle ");	
	if(fprintf(child_handle,"%10.2lf %10.2lf %10.2lf %10.2lf", (double) c_sys_time / CLOCK, (double) c_user_time / CLOCK, (double) (c_sys_time + c_user_time) / CLOCK, (double) c_real_time/MACRO_SECONDS) < 0) perror("time_checker fprintf error child_handle ");	
	if(fprintf(sum_handle,"%10.2lf %10.2lf %10.2lf %10.2lf", (double) (p_sys_time + c_sys_time) / CLOCK, (double) (p_user_time + c_user_time) / CLOCK, (double) (p_sys_time + p_user_time + c_sys_time + c_user_time) / CLOCK, ( (double)p_real_time / CLOCK + (double)c_real_time /MACRO_SECONDS)) < 0) perror("time_checker fprintf error parent_handle ");	
	
}


suseconds_t test_fork(int n){
	pid_t pid;
	suseconds_t total_real_time = 0;
	struct timeval t;
	int i;
	for(i = 0; i < n; i++){
		gettimeofday(&t, NULL);
		suseconds_t partial_real_time = t.tv_usec; 
		if( (pid = fork()) < 0) print_error(errno,"fork error");
		else if(pid == 0){
			counter ++;
			_exit(0);
		}
		else {
			wait(NULL);
			gettimeofday(&t, NULL);
			partial_real_time > t.tv_usec ? (partial_real_time = MACRO_SECONDS - partial_real_time + t.tv_usec) : (partial_real_time = t.tv_usec - partial_real_time);
		//	printf("\n %ld", partial_real_time);
			total_real_time += partial_real_time;
		}
	}
return total_real_time;
}


suseconds_t test_vfork(int n){
	pid_t pid;
	suseconds_t total_real_time = 0;
	struct timeval t;
	int i;
	for(i = 0; i < n; i++){
		gettimeofday(&t, NULL);
		suseconds_t partial_real_time = t.tv_usec; 
		if( (pid = vfork()) < 0) print_error(errno,"vfork error");
		else if(pid == 0){
			counter ++;
			_exit(0);
		}
		else {
			wait(NULL);
			gettimeofday(&t, NULL);
			partial_real_time > t.tv_usec ? (partial_real_time = MACRO_SECONDS - partial_real_time + t.tv_usec) : (partial_real_time = t.tv_usec - partial_real_time);
//			printf("\n %ld", partial_real_time);
			total_real_time += partial_real_time;
		}
	}
return total_real_time;
}

//long test_fork(int n){
//	pid_t pid;
//	long total_real_time = 0;
//	struct timespec t;
//	int i, status;
//	for(i = 0; i < n; i++){
//		if( (pid = fork()) < 0) print_error(errno,"fork error");
//		else if(pid == 0){
//			clock_gettime(CLOCK_MONOTONIC, &t);
//			long partial_real_time = t.tv_nsec; 
//			counter ++;
//			clock_gettime(CLOCK_MONOTONIC, &t);
//			_exit(t.tv_nsec - partial_real_time);
//		}
//		else {
//			wait(&status);
//			total_real_time += WEXITSTATUS(status);
//		}
//	}
//return total_real_time;
//}



int fn(void *arg){
	counter ++;
	_exit(0);
}

suseconds_t test_clone_fork(int n){
	pid_t pid;
	int i = 0;
	void *stack = malloc(STACK_SIZE);	
	suseconds_t total_real_time = 0;	
	struct timeval t;
	for(i = 0; i<n; i++){		
		gettimeofday(&t, NULL);
		suseconds_t partial_real_time = t.tv_usec; 
		if( (pid = clone(&fn, stack + STACK_SIZE, SIGCHLD, NULL)) < 0) print_error(errno,"clone_fork error");
		wait(NULL);
		gettimeofday(&t, NULL);
		partial_real_time > t.tv_usec ? (partial_real_time = MACRO_SECONDS - partial_real_time + t.tv_usec) : (partial_real_time = t.tv_usec - partial_real_time);
		total_real_time += partial_real_time;
	}	
free(stack);
return total_real_time;
}



suseconds_t test_clone_vfork(int n){
	pid_t pid;
	int i = 0;
	void *stack = malloc(STACK_SIZE);	
	suseconds_t total_real_time = 0;	
	struct timeval t;
	for(i = 0; i<n; i++){		
		gettimeofday(&t, NULL);
		suseconds_t partial_real_time = t.tv_usec; 
		if( (pid = clone(&fn, stack + STACK_SIZE, SIGCHLD | CLONE_VFORK | CLONE_VM, NULL)) < 0) print_error(errno,"clone_fork error");
		wait(NULL);
		gettimeofday(&t, NULL);
		partial_real_time > t.tv_usec ? (partial_real_time = MACRO_SECONDS - partial_real_time + t.tv_usec) : (partial_real_time = t.tv_usec - partial_real_time);
		total_real_time += partial_real_time;
	}	
free(stack);
return total_real_time;
}


//long test_clone_fork(int n){
//	void *stack = malloc(STACK_SIZE);	
//	pid_t pid;
//	long total_real_time = 0;
//	int i, status;
//	for(i = 0; i<n; i++){		
//		if( (pid = clone(&fn, stack + STACK_SIZE, SIGCHLD, NULL)) < 0) print_error(errno,"clone_fork error");
//		wait(&status);
//		total_real_time += WEXITSTATUS(status);
//	}	
//free(stack);
//return total_real_time;
//}



int main(int argc, char *argv[]){
	int number_of_cycles[] = {50000,100000,150000,200000};
	int i = 0;
	

//	for(i = 0; i<1;i++){
//		printf("\n counter after fork %d, real_time %ld ", counter, test_fork(number_of_cycles[i]));
//		counter = 0;		
//		printf("\n counter after vfork %d, real_time %ld ", counter, test_vfork(number_of_cycles[i]));
//		counter = 0;
//		printf("\n counter after clone_fork %d, real_time %ld ", counter, test_clone_fork(number_of_cycles[i]));
//		counter = 0;
//		printf("\n counter after clone_vfork %d, real_time %ld ", counter, test_clone_vfork(number_of_cycles[i]));
//	}

	FILE *parent_handle;
	if( (parent_handle = fopen("data/parent","w")) < 0) perror("fopen failed");
	FILE *child_handle;
	if( (child_handle = fopen("data/child","w")) < 0) perror("fopen failed");
	FILE *sum_handle;
	if( (sum_handle = fopen("data/sum","w")) < 0) perror("fopen failed");
	fprintf(parent_handle,"%10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s","N","sys_f","usr_f","syusr_f","real_f","syst_f","usr_vf","syusr_vf","real_vf","sys_cf","usr_cf","syusr_cf","real_cf","sys_cvf","usr_cvf","syusr_cvf","real_cvf");
	fprintf(child_handle,"%10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s","N","sys_f","usr_f","syusr_f","real_f","syst_f","usr_vf","syusr_vf","real_vf","sys_cf","usr_cf","syusr_cf","real_cf","sys_cvf","usr_cvf","syusr_cvf","real_cvf");
	fprintf(sum_handle,"%10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s","N","sys_f","usr_f","syusr_f","real_f","syst_f","usr_vf","syusr_vf","real_vf","sys_cf","usr_cf","syusr_cf","real_cf","sys_cvf","usr_cvf","syusr_cvf","real_cvf");
	for(i = 0; i<4; i++){
		struct tms *t = malloc(sizeof(struct tms));
		fprintf(parent_handle,"\n%10d ", number_of_cycles[i]);
		fprintf(child_handle,"\n%10d ", number_of_cycles[i]);
		fprintf(sum_handle,"\n%10d ", number_of_cycles[i]);
		counter = 0;
		clock_t p_real_time = times(t);
		suseconds_t c_real_time = test_fork(number_of_cycles[i]);
		time_checker(t, c_real_time, p_real_time, parent_handle, child_handle, sum_handle); 		
	
		p_real_time = times(t); 
		counter = 0;	
		c_real_time = test_vfork(number_of_cycles[i]);
		time_checker(t, c_real_time, p_real_time, parent_handle, child_handle, sum_handle); 		
		
		p_real_time = times(t);
		counter = 0;
		c_real_time = test_clone_fork(number_of_cycles[i]);
		time_checker(t, c_real_time, p_real_time, parent_handle, child_handle, sum_handle); 		
		
		p_real_time = times(t);
		counter = 0;
		c_real_time = test_clone_vfork(number_of_cycles[i]);
		time_checker(t, c_real_time, p_real_time, parent_handle, child_handle, sum_handle); 		
	}


	

return 0;
}
