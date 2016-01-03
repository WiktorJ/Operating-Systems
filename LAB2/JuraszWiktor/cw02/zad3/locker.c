#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
void print_error(int error, char* message){
		puts(message);
		puts(strerror(error));
}

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len, struct flock *lock){
	lock->l_type = type;
	lock->l_start = offset;
	lock->l_whence = whence;
	lock->l_len = len;
	return( fcntl(fd, cmd, lock));
}

int set_lock(int handle, short lock_type, int number){
	struct flock *lock = malloc(sizeof(struct flock));
	if(number >= lseek(handle, 0, SEEK_END)) return 1;
	return lock_reg(handle, F_SETLK, lock_type , number ,SEEK_SET, 1, lock); 	
}

void list_locks(int handle){
	int file_size =lseek(handle, 0, SEEK_END);
	int i;
	struct flock *lock = malloc(sizeof(struct flock));
	for(i = 0; i < file_size; i++){
		if (lock_reg(handle, F_GETLK, F_RDLCK, i, SEEK_SET, 1, lock) == -1) print_error(errno, "list locks read lock error"); 
		else if(lock->l_pid == -1) printf("\n L_GETLK for read lock failed, pid == -1");
		else if (lock->l_type != F_UNLCK) printf("\n for character number %d lock of type: L_RDLCK with pid: %ld", (int) lock->l_start, (long) lock->l_pid);		
	}

	for(i = 0; i< file_size; i++){
		if (lock_reg(handle, F_GETLK, F_WRLCK, i, SEEK_SET, 1, lock) == -1) print_error(errno, "list locks write lock error");
		else if(lock->l_pid == -1) printf("\n L_GETLK for write lock failed, pid == -1");
		else if(lock->l_type != F_UNLCK) printf("\nfor character number %d lock of type: L_WRLCK with pid: %ld", (int) lock->l_start , (long) lock->l_pid);
	}
}	


void read_character(int handle, int number){
	char character;
	if(number >= lseek(handle, 0, SEEK_END)) printf("\nnumber of character to read points to memory area, that is out of file");
	else if(lseek(handle, number, SEEK_SET)==-1) print_error(errno, "lseek error while getting character position");
	else if(read(handle, &character, 1) == -1) print_error(errno, "read error while reading character");
	else printf("\nread character: %c", character);			
}	


void overwrite_character(int handle, int number, char character){
	printf("\n--%d--",number);
	if(number >= lseek(handle, 0, SEEK_END)) printf("\nnumber of character to overwrite points to memory area, that is out of file");
	else if(lseek(handle, number, SEEK_SET)==-1) print_error(errno, "lseek error while getting character position");
	else if(write(handle, &character, 1) == -1) print_error(errno, "write error while overwriting character");
	else printf("\noverwriting done");			
}	

int main(int argc, char *argv[]){
	char options[7][30] ={
		"1: set readlock to character",
		"2: set writelock to character",
		"3: list locked characters",
		"4: free lock",
		"5: read character",
		"6: change character",
		"7: exit\n\n"
	};
	if(argc != 2){
		print_error(errno,"bad number of arguments");
		exit(EXIT_FAILURE);
	}

	int handle = open(argv[1], O_RDWR);

	if(handle < 0){
		print_error(errno, "open error");
		exit(EXIT_FAILURE);
	}

	int flag = 1;
	while(flag == 1){
		printf("\nchoose operation: ");
		int i;
		for(i = 0; i< 7; i++){
			printf("\n%s",options[i]);
		}
		int choice, number;
		char character;
		scanf("%d",&choice);
		switch(choice){
			case 1:
				printf("\nenter character number to readlock (counting from the file begining): ");
				scanf("%d",&number);
				if(set_lock(handle, F_RDLCK, number) == -1) print_error(errno,"error during setting readlock");
			break;
			case 2:
				printf("\nenter character number to writelock (counting from the file begining): ");
				scanf("%d", &number);
				if(set_lock(handle, F_WRLCK, number) == -1) print_error(errno,"error during setting writelock");
			break;
			case 3:
				list_locks(handle);	
			break;
			case 4:
				printf("\nenter character number to unlock (counting from the file begining): ");
			        scanf("%d", &number);
				if(set_lock(handle,F_UNLCK, number) == -1) print_error(errno, "error during unlocking");	
			break;
			case 5:
				printf("\nenter character number to read (counting from the file begining): ");
				scanf("%d", &number);
				read_character(handle, number);
			break;
			case 6:
				printf("\nenter character number to overwrite (counting from the file begining): ");
				scanf("%d", &number);				
				printf("\naaaenter character to overwrite with: ");
				scanf("%c",&character);
			        scanf("%c", &character);
				overwrite_character(handle, number, character);	
			break;
			default: 
				flag = 0;
			break;
		}
	
	}
	close(handle);
	return 0;
}


