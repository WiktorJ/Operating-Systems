#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

#define RECORD_SIZE 1024
#define RECORD_NO 10
void err_sys(const char* x){ 
    perror(x); 
    exit(EXIT_FAILURE); 
}

struct record{
    int id;
    char text[RECORD_SIZE-4];
};

void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                     
   while (length-- > 0) {
	   size_t index = rand() % (sizeof charset - 1);
	   *dest++ = charset[index];
   }
}

void generate(char* filename, int n){
    int check;
    FILE *fp; 
    if ((fp=fopen(filename, "w"))==NULL) {      //"w" - otwiera plik do nadpisywania (zamazuje starą treść)
        err_sys("Opening failed");
    }
    
	time_t t;
	srand((unsigned) time(&t));
    
    int i;
    for (i = 0; i < n; i++){ 
        struct record *rec = malloc(sizeof(struct record));
        rec->id = i;
        rand_str(rec->text, 1020);
        fwrite(rec, sizeof(struct record), 1, fp);
    }
     
    check = fclose (fp);
    if (check  == -1)
		err_sys("Closing failed\n");
}

void readFile(char *filename){
    int check;
    FILE *fp; 
    if ((fp=fopen(filename, "r"))==NULL) {      //"w" - otwiera plik do nadpisywania (zamazuje starą treść)
        err_sys("Opening failed");
    }
    
    int i;
    for (i = 0; i < RECORD_NO; i++){ 
        struct record *str = malloc(sizeof(struct record));
        fread(str, sizeof(struct record), 1, fp);
        printf("%d\t%s\n", str->id, str->text);
    }
     
    check = fclose (fp);
    if (check  == -1)
		err_sys("Closing failed\n");
}

int main (int argc, char *argv[]){
    
    if (argc != 2)
        err_sys("\nWrong arguments format! \nCorrect syntax:      ./zad1 filename\n\n");
        
    char *filename = argv[1];
    
    generate(filename, RECORD_NO);
    
    readFile(filename);
    
    exit(EXIT_SUCCESS);
}
