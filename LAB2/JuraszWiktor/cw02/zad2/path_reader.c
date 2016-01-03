#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
void print_error(int error, char* message){
		puts(message);
		puts(strerror(error));
}

int compare_permissions(int given_permission, mode_t found_permission){
	found_permission %= 01000;
	return (given_permission == found_permission) ? 1:0;
}


int permission_oct(char *char_perm){
	if(strlen(char_perm) != 3){
		printf("WRONG PERMISSSION FORMAT");
		return 1;
	}

	int permission = 0;
	int oct_val = 0100;
	int i;

	for(i = 0; i<3; i++){
		switch(char_perm[i]){
			case '1': permission += oct_val;
				break;
			case '2': permission += 2*oct_val;
				break;
			case '3': permission += 3*oct_val;
				break;			
			case '4': permission += 4*oct_val;
				break;
			case '5': permission += 5*oct_val;
				break;
			case '6': permission += 6*oct_val;
				break;			
			case '7': permission += 7*oct_val;
				break;
			default:
				if(char_perm[i] != '0') printf("WRONG PERMISION FORMAT");
				break;
		}
		oct_val = oct_val / 010;
	}
	return permission;
}

void read_path(char *path, int permission){
	DIR *dp;
	if((dp=opendir(path)) == NULL){
	       	print_error(errno,"open dir error");
		return;
	}
	struct stat *buf = malloc(sizeof(struct stat));
	struct dirent *dirp;	
	while((dirp = readdir(dp))){
		char tmp_path[strlen(path) + strlen(dirp -> d_name) + 2];
		strcpy(tmp_path, path);
		strcat(tmp_path, "/");
		strcat(tmp_path, dirp -> d_name);
		lstat(tmp_path, buf); 
		if(strcmp(dirp-> d_name, ".\0")==0 || strcmp(dirp -> d_name, "..\0")==0) continue;
		if(S_ISDIR(buf->st_mode)){
			read_path(tmp_path, permission);	
		}	
		else if(S_ISREG(buf->st_mode)){
			if(compare_permissions(permission, buf->st_mode)){
				printf("\nFILE NAME: %s, LAST MODIFICATION: %s, SIZE: %lu", dirp -> d_name , ctime(&buf->st_mtime), buf-> st_blocks*512);   				
			}
		}
	}
	closedir(dp);
}


int main(int argc, char *argv[]){

	if(argc != 3) {
		printf("\nbad number of arguments");
		exit(EXIT_FAILURE);
	}	
	int permission = permission_oct(argv[2]);
	read_path(argv[1], permission);
	printf("\n");
	return 0;
}
