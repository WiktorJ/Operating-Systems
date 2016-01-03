#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include <time.h>
char * itoa(int number){
	int tmp = number;
	int i =1;
	char const digit[] = "0123456789";
	while(tmp > 9){
		tmp = tmp / 10;
		i++;
	}
	char *str = malloc(sizeof(char)*i);
	char *tmp_str = str;
	while(i){
		++tmp_str;
		i--;	
	}
	*tmp_str = '\0';
	
	
	do{
		*--tmp_str = digit[number%10];
		number = number / 10;
	}while(number);
	return str;
	
}

void time_checker(struct tms *exe_times, int records_number, int record_size, FILE *handle, char *alg_name){
	clock_t  prev_user = exe_times -> tms_utime;
	clock_t  prev_sys = exe_times -> tms_stime;
	times(exe_times);
	char *records_number_to_write = itoa(records_number);
	char *record_size_to_write = itoa(record_size);
	char *user_time_to_write = itoa(exe_times->tms_utime - prev_user);
	char *system_time_to_write = itoa(exe_times->tms_stime - prev_sys);
	char *records_number_string = "\nNUMBER OF RECORDS: ";
	char *record_size_string = "\nRECORD SIZE: ";
	char *system_time = "\nSYSTEM TIME: ";
	char *user_time = "\nUSER TIME: ";
	if(fwrite(alg_name, sizeof(char), strlen(alg_name), handle) != strlen(alg_name)) perror("time_checker write error");	
	
	if(fwrite(records_number_string, sizeof(char), strlen(records_number_string), handle) != strlen(records_number_string)) perror("time_checker write error");	
	if(fwrite(records_number_to_write, sizeof(char), strlen(records_number_to_write), handle) != strlen(records_number_to_write)) perror("time_checker write error");
	
	if(fwrite(record_size_string, sizeof(char), strlen(record_size_string), handle) != strlen(record_size_string)) perror("time_checker write error");
	if(fwrite(record_size_to_write, sizeof(char), strlen(record_size_to_write), handle) != strlen(record_size_to_write)) perror("time_checker write error");	

	if(fwrite(system_time, sizeof(char), strlen(system_time), handle) != strlen(system_time)) perror("time_checker write error");
	if(fwrite(system_time_to_write, sizeof(char), strlen(system_time_to_write), handle) != strlen(system_time_to_write)) perror("time_checker write error");
	
	if(fwrite(user_time, sizeof(char), strlen(user_time), handle) != strlen(user_time)) perror("time_checker write error");
	if(fwrite(user_time_to_write, sizeof(char), strlen(user_time_to_write), handle) != strlen(user_time_to_write)) perror("time_checker write error");
}

void print_error(int error, char* message){
	puts(message);
	puts(strerror(error));
}

void sys_bubble_sort(int handle, int iteration_number, int record_size){
	int i,j,n1,n2;
	char buf1[record_size];
	char buf2[record_size];
	for(i = 0; i < iteration_number; i++){
		if(lseek(handle, 0, SEEK_SET) == -1)print_error(errno,"sys_bubble_sort lseek error 0");	
		for(j = 0; j<iteration_number; j++){
			n1 = read(handle, buf1, record_size);
			n2 = read(handle, buf2, record_size);
			if(n1<0 || n2<0) print_error(errno, "sys_bubble_sort read error");
			else {
				char copy1[1];
				char copy2[1];
				strncpy(copy1, buf1, 1);
				strncpy(copy2, buf2, 1);
				if(strcmp(copy1, copy2) > 0){
					if(lseek(handle, -2*record_size, SEEK_CUR) == -1)print_error(errno,"sys_bubble_sort lseek error 1");	
					if(write(handle, buf2, record_size) != record_size) print_error(errno,"sys_bubble_sort write error 1");
				        if(write(handle, buf1, record_size) != record_size) print_error(errno,"sys_bubble_sort write error 2");	
				}
			}
				
			if(lseek(handle, -record_size, SEEK_CUR) == -1)print_error(errno,"sys_bubble_sort lseek error 2");	
		}
	}
}




void lib_bubble_sort(FILE *handle, int iteration_number, int record_size){
	int i,j,n1,n2;
	char buf1[record_size];
	char buf2[record_size];
	for(i = 0; i < iteration_number; i++){
		if(fseek(handle, 0, SEEK_SET) == -1)perror("lib_bubble_sort lseek error 0");	
		for(j = 0; j<iteration_number; j++){
			n1 = fread(buf1, sizeof(char), record_size, handle);
			n2 = fread(buf2, sizeof(char), record_size, handle);
			if(n1<0 || n2<0) perror("lib_bubble_sort read error");
			else {
				char copy1[1];
				char copy2[1];
				strncpy(copy1, buf1, 1);
				strncpy(copy2, buf2, 1);
				if(strcmp(copy1, copy2) > 0){
					if(fseek(handle,  -2*record_size, SEEK_CUR) == -1)perror("lib_bubble_sort lseek error 1");	        
					if(fwrite(buf2, sizeof(char), record_size, handle) != record_size) perror("lib_bubble_sort write error 2");
					if(fwrite(buf1, sizeof(char), record_size, handle) != record_size) perror("lib_bubble_sort write error 2");	
				}
			}
				
			if(fseek(handle, - record_size, SEEK_CUR) == -1)perror("lib_bubble_sort lseek error 2");	
		}
	}
}


int main(int argc, char *argv[]){
	*argv++;
	char *file_path = *argv++;
	int record_size = atoi(*argv);
	int handle = open(file_path, O_RDWR);
	if(handle<0){
		int err =errno;
		puts("open failed");
		puts(strerror(err));
		exit(EXIT_FAILURE);
	}
	int file_size = lseek(handle, 0, SEEK_END);
	if(file_size == -1) print_error(errno,"seek error");
	lseek(handle, 0, SEEK_SET);
	
	FILE *stat_file =fopen("statystyki", "a");
	if(stat_file == NULL) {
		perror("fopen error");
		exit(EXIT_FAILURE);
	}
	
	int iteration_number = file_size/record_size;
	struct tms *exe_times = (struct tms*)malloc(sizeof(struct tms));
	times(exe_times);
	sys_bubble_sort(handle, iteration_number, record_size);
	time_checker(exe_times, iteration_number, record_size, stat_file, "\n--------------------------\n---------------------\nSYSTEM_BUBBLE: \n");
	if(close(handle)==-1)print_error(errno,"close error");
	char *copy_name = malloc(sizeof(char)*(strlen(file_path + 5)));
	strcat(copy_name,file_path);
	strcat(copy_name,"_copy");

	FILE *file =fopen(copy_name, "r+b");
	if(file == NULL) {
		perror("fopen error");
		exit(EXIT_FAILURE);
	}
	times(exe_times);
	lib_bubble_sort(file, iteration_number, record_size);
	time_checker(exe_times, iteration_number, record_size, stat_file, "\n--------------------------\n---------------------\nLIBRARY_BUBBLE: \n");
	char end[39] = "\n************\n************\n************";	
	if(fwrite(end, sizeof(char), 39, stat_file) != 39) perror("time_checker write error");	
	fclose(file);
	exit(EXIT_SUCCESS);
}
