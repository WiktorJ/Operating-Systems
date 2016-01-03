#include <time.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/errno.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#define RECORD_SIZE 1024
void print_error(int error, char* message){
    puts(message);
    puts(strerror(error));
}

void print_error_exit(int error, char* message){
    puts(message);
    puts(strerror(error));
    exit(EXIT_FAILURE);
}

void * search_file(void * text_to_find);
int file_desc;
pthread_t *threads;
pthread_mutex_t p_mutex;
int records_number;
int threads_number;
void atexit_function(){
    free(threads);
    close(file_desc);
}
struct record{
    int id;
    char text[RECORD_SIZE - sizeof(int)];
};
int main(int argc, char **argv){

    atexit(atexit_function);

    if(argc != 5){
        printf("usage: ./records_counter <file name> <threads number> <records number> <searched word>");
        exit(EXIT_FAILURE);
    }
    
    if( (file_desc = open(argv[1], O_RDONLY)) == -1)
        print_error_exit(errno, "can not open file");

    if( (pthread_mutex_init(&p_mutex, NULL)) !=0 )
        print_error_exit(errno, "error in pthread_mutex_init");

    threads_number = atoi(argv[2]);
    records_number = atoi(argv[3]);

    threads = malloc( sizeof(pthread_t) *threads_number);
    int i;
    for(i = 0; i < threads_number; i++){
        if(pthread_create(&threads[i], NULL, search_file, argv[4]) == -1);
    }
    
    for(i = 0; i < threads_number; i++){
        if(pthread_join(threads[i], NULL) == -1)
            print_error(errno, "error in pthread_join");
    }
    
    exit(EXIT_SUCCESS);
}

void* search_file(void * text_to_find){
    if(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) !=0)    
        print_error(errno, "pthread_setcanceltype error");
    struct record * buffer;
    int bytes_read = 1;
    int file_offset;
    buffer = malloc( sizeof(char)*RECORD_SIZE + 1);
    int finish_flag = 0;
    while(bytes_read > 0 && finish_flag == 0){
        pthread_mutex_lock(&p_mutex);
        int j;
        for(j = 0; j < records_number; j++){
            if( (file_offset = lseek(file_desc, 0, SEEK_CUR)) == -1)
                print_error(errno, "erron in lseek");
            if( (bytes_read = read(file_desc, buffer, RECORD_SIZE)) == -1)
                print_error(errno, "errno in read");
            buffer->text[RECORD_SIZE-sizeof(int)] = 0;
            //printf("id: %d\n%s\n",buffer->id,buffer->text);
            //printf("%d\n", bytes_read);
            char *substr;
            substr = strstr(buffer->text, text_to_find);
            if(substr != NULL){
                int i;
                for(i = 0; i < threads_number; i++){
                    if(threads[i] != pthread_self())
                        pthread_cancel(threads[i]);
                }
                int line = file_offset/1024;
                printf("id of thread: %ld   id of record: %d\n", (long)pthread_self(), line +1);
                finish_flag = 1;
                break;
            }
        }
        pthread_mutex_unlock(&p_mutex);


    }
    return NULL;
}
