#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/wait.h>
void print_error(int error, char* message){
	puts(message);
	puts(strerror(error));
	fflush(stdout);
}



int main(int argc, char *argv[]){
	char *path;
	char *options;
	if(argc > 3 || argc < 2){
		printf("bad number of arguments");
		return -1;
	}
	else{
//		printf("arg1 %s, arg2 %s", argv[1], argv[2]);
//		fflush(stdout);
		path = argv[1];
		options = "-f";
		if(argc == 3) options = argv[2];
	}
	
	DIR *dp;
//	printf("\n ARG0: %s", argv[2]);
//	fflush(stdout);
	if( (dp = opendir(path)) == NULL){
		print_error(errno, "oped dir error");
		return -1;
	}

	int w_flag = 0;
	int p_flag = 0;
	int c;
	int files_counter = 0;
	int sub_dir_counter = 0;

	while ( (c = getopt(argc, argv, "wpf")) != -1){
		switch(c)
		{
			case 'w':
				w_flag = 1;
			break;
			case 'p':
				p_flag = 1;
			break;
			case 'f':
				p_flag = w_flag = 0;
			break;
			default :
				printf("wrong arguments");
				fflush(stdout);
				return -1;
			break;
		}
	}

	struct stat *buf = malloc(sizeof(struct stat));
	struct dirent *dirp;
	pid_t pid;
	while( (dirp = readdir(dp))){
		char tmp_path[strlen(path) + strlen(dirp -> d_name) + 2];
        	strcpy(tmp_path, path);
		strcat(tmp_path, "/");
		strcat(tmp_path, dirp -> d_name);
	//	printf("\n %s", tmp_path);
	//	fflush(stdout);
		lstat(tmp_path, buf);
		if(strcmp(dirp-> d_name, ".\0")==0 || strcmp(dirp -> d_name, "..\0")==0) continue;
        	if(S_ISDIR(buf->st_mode)){
        		sub_dir_counter++;
			if( (pid = fork()) < 0){
				print_error(errno,"fork errok");
				_exit(-1);
			}
			else if(pid == 0){
				char *args[] = {argv[0], tmp_path, options, NULL};
	//			printf("\n TMP: %s ARG1: %s", tmp_path, argv[2]);
	//			fflush(stdout);
				if( (execv(args[0],args)) < 0) print_error(errno, "execv error");
			}
        	}
        	else if(S_ISREG(buf->st_mode)){
        		files_counter++;		
		}
	}
	closedir(dp);
	if(w_flag) sleep(15);
	int sub_dirs_files = 0;
	while (sub_dir_counter > 0){
		int status, sub_dir_files;
		wait(&status);
		if( (sub_dir_files = WEXITSTATUS(status)) > 0) sub_dirs_files += sub_dir_files;
		sub_dir_counter--;			
	}
	if(p_flag){
		printf("\nPATH: %s FILES: %d FILES+SUB_FILES: %d", path, files_counter, files_counter + sub_dirs_files);
		fflush(stdout);
	}
	 _exit(files_counter + sub_dirs_files);

}
