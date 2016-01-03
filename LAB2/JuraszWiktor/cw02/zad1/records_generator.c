#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void print_error(int error, char* message){
	puts(message);
	puts(strerror(error));
}


char* randomString(size_t length) {
	char *returnValue = malloc(sizeof(char) * length);
	char *dest = returnValue;
	char charset[] = "0123456789"
      		        "abcdefghijklmnopqrstuvwxyz"
           		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	while (length-- > 0) {
		size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
		*dest++ = charset[index];
	}
	*dest++ = '\n';
	*dest = '\0';
	return returnValue;
}


int main(int argc, char *argv[]){
	int handle;
	int handle2;
	*argv++;
	char *file_name = *argv++;
	char *copy_name = malloc(sizeof(char)*(strlen(file_name + 5)));
	strcat(copy_name,file_name);
	strcat(copy_name,"_copy");
	printf("\n copy name: %s \n",copy_name);
	if((handle = open(file_name, O_RDWR | O_CREAT |O_APPEND | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR)) < 0)	
		print_error(errno, "can not open/create file");
	if((handle2 = open(copy_name, O_RDWR | O_CREAT |O_APPEND | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR)) < 0)
		print_error(errno, "can not open/create copy file");
	int size = atoi(*argv++);
	int records_number = atoi(*argv);
	int i;
	for(i = 0; i < records_number; i++){
		if(write(handle,randomString(size-1), size) != size) print_error(errno, "blad zapisu");
	
	}
	int n;
	char buf[size];
	lseek(handle, 0, SEEK_SET);
	while( (n = read(handle, buf, size)) > 0)
			if(write(handle2, buf, n) !=n)
				print_error(errno, "blad kopiowania");
	
	if(n<0)
		print_error(errno, "blad czytania");

	return 0;
}
