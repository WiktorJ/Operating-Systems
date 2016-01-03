#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#define M_KEY 18
#define SIZE_OF_ARRAY 10
#define TIME_TO_SLEEP 10000000
struct wrapper
{
        int array[SIZE_OF_ARRAY];
        sem_t empty;
        sem_t pmutex;
        sem_t cmutex;
        sem_t full;
        int last_produced;
        int started;
};

int memory_id;
struct wrapper *memory;
int producer = 0;
char *key_path = ".";

int is_prime(int number){
    double s = sqrt((double)number);
    int i;
    for(i = 2; i <= (int)floor(s); i++){
        if( (number % i) == 0) 
            return 0;
    }
    return 1;
}

void print_error(int error, char* message){
    puts(message);
    puts(strerror(error));
}

void print_error_exit(int error, char* message){
    puts(message);
    puts(strerror(error));
    exit(EXIT_FAILURE);
}

void atexit_function(){
    if(shmctl(memory_id,IPC_RMID,NULL) == -1)
        print_error(errno, "shmctl error with IPC_RMID flag");
    
    if(shmdt(memory) == -1);
        print_error(errno, "shmdtl error");

    if(sem_destroy(&memory->pmutex) == -1)
        print_error(errno, "sem_destroy pmutex error");

    if(sem_destroy(&memory->cmutex) == -1)
        print_error(errno, "sem_destroy cmutex error");
    
    if(sem_destroy(&memory->empty) == -1)
        print_error(errno, "sem_destroy empty error");
    
    if(sem_destroy(&memory->full) == -1)
        print_error(errno, "sem_destroy full error");
}

int main(int argc, char** argv){
    
    if(argc != 3)
        print_error_exit(errno, "usage: ./producer_or_consumer <producer or consumer flag p/c> <initialize semaphore flag 0/1>");


    if(strcmp(argv[1],"c") == 0)
        producer = 0;
    else if(strcmp(argv[1],"p") == 0)
        producer = 1;

    

     
    atexit(atexit_function);
    key_t  memory_key;
    if( (memory_key = ftok(key_path, M_KEY)) == -1)
        print_error_exit(errno,"ftok error");

    if( (memory_id = shmget(memory_key, sizeof(struct wrapper), IPC_CREAT | 0600)) == -1)
            print_error_exit(errno, "shmget error");
    

    if( (memory = shmat(memory_id, NULL, 0)) == (void*)-1)
        print_error_exit(errno, "shmat error");

    
    //if(memory->started!=10) {
        memset(&memory->array, 1, sizeof(memory->array));
        sem_init(&memory->empty, 1, SIZE_OF_ARRAY);
        sem_init(&memory->pmutex, 1, 1);
        sem_init(&memory->cmutex, 1, 1);
        sem_init(&memory->full, 1, 0);
        memory->last_produced = -1;
        memory->started=10;
      //  }

    if(producer){
        while(1){
         sem_wait(&memory->empty);
        // sem_wait(&memory->pmutex);
         time_t t = time(NULL);
         char *string_time = ctime(&t);
         int r =rand();
         
         memory->last_produced++;
         memory->array[memory->last_produced]=r;
         
         printf("PID:%d#TIME_STAMP%s#ADDED: %d#REMAINING_TASKS %d: ",getpid(), string_time, r, memory->last_produced+1);    
         usleep(1000000);
        // sem_post(&memory->pmutex);
         sem_post(&memory->full);
        }
    } else {
        while(1){
             sem_wait(&memory->full);
          //   sem_wait(&memory->cmutex);
             time_t t = time(NULL);
             char *string_time = ctime(&t);
             int r = memory->array[memory->last_produced];
             memory->last_produced--;
             if(is_prime(r))
                printf("PID:%d#TIME_STAMP%s#CHECKED NUMBER: %d - IS_PRIME #REMAINING_TASKS %d: ",getpid(), string_time, r, memory->last_produced+1);    
             else
                printf("PID:%d#TIME_STAMP%s#CHECKED NUMBER: %d - IS_NOT_PRIME #REMAINING_TASKS %d: ",getpid(), string_time, r, memory->last_produced+1);    
             usleep(1000000);
            //sem_post(&memory->cmutex);
            sem_post(&memory->empty);
        }
    
    
    
    
}}
