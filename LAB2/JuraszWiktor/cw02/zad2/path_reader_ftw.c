#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <time.h>
int permission;

void permission_oct(char *char_perm){
	if(strlen(char_perm) != 3){
		printf("WRONG PERMISSSION FORMAT");
		return;
	}

	permission = 0;
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
}


	
	

int fn(const char *fpath, const struct stat *sb, int typeflag){

	int per = sb -> st_mode %01000;
	//printf("\nmy prem = %o and st prem = %o", permission,per);
	
	if(typeflag == FTW_F && permission == per)  printf("\nFILE NAME: %s, LAST MODIFICATION: %s , SIZE: %lu", fpath ,ctime(&sb->st_mtime), sb-> st_blocks*512);
	return 0;
}

int main(int argc, char *argv[]){
	permission_oct(argv[2]);
	if(ftw(argv[1], &fn, 1) == -1) printf("FTW ERROR");
	printf("\n");
	return 0;
}
