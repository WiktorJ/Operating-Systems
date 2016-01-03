#include <time.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/errno.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#define M_KEY 1
#define S_KEY 3
#define SHARED_MEMORY_SIZE 1024
#define S_PRODUCER 0
#define S_CONSUMER 1
#define SIZE_OF_ARRAY 10
struct wrapper{
        int array[SIZE_OF_ARRAY];
        int last_produced;
};

union semun {
        int              val;    /* Value for SETVAL */
        struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
        unsigned short  *array;  /* Array for GETALL, SETALL */
        struct seminfo  *__buf;  /* Buffer for IPC_INFO */
};

int semaphore;
int memory_id;
struct wrapper * memory;
int creator = 0;
int producer = 0;
    int rc;
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
    
    if(semctl(semaphore,0,IPC_RMID) == -1)
        print_error(errno, "semctl error with IPC_RMID flag");
    
    if(shmdt(memory) == -1);
        print_error(errno, "shmdtl error");
}

int main(int argc, char** argv){
    
    if(argc != 3)
        print_error_exit(errno, "usage: ./producer_or_consumer <producer or consumer flag p/c> <initialize semaphore flag 0/1>");


    if(strcmp(argv[1],"c") == 0)
        producer = 0;
    else if(strcmp(argv[1],"p") == 0)
        producer = 1;

    
    if(strcmp(argv[2],"0") == 0)
        creator= 0;
    else if(strcmp(argv[2],"1") == 0)
        creator = 1;

     
    atexit(atexit_function);
    key_t semaphore_key, memory_key;
    if( ((semaphore_key = ftok(key_path, S_KEY)) == -1) || ((memory_key = ftok(key_path, M_KEY)) == -1))
        print_error_exit(errno,"ftok error");

    if( (memory_id = shmget(memory_key, sizeof(struct wrapper), IPC_CREAT | 0600)) == -1)
            print_error_exit(errno, "shmget error");
    

    if( (memory = shmat(memory_id, NULL, 0)) == (void*)-1)
        print_error_exit(errno, "shmat error");

    if( (semaphore = semget(semaphore_key, 2, 0600)) == -1){
        if(errno == ENOENT){
            if( (semaphore = semget(semaphore_key, 2, IPC_CREAT | 0600)) == -1)
                print_error_exit(errno, "semget error with IPC_CREAT FLAG");
        } else {
            print_error_exit(errno, "semget error");
        }        
    }

    union semun semaphore_arg;
    

    if(creator){
        semaphore_arg.val = SIZE_OF_ARRAY;
        if(semctl(semaphore, S_PRODUCER, SETVAL, semaphore_arg) == -1)
            print_error_exit(errno, "semctl errno");
        semaphore_arg.val = 0;
        if(semctl(semaphore, S_CONSUMER, SETVAL, semaphore_arg) == -1)
            print_error_exit(errno, "semctl errno");
        memory->last_produced = -1;
    }


    if(producer){
        struct sembuf producer_semaphore, consumer_semaphore;
        producer_semaphore.sem_flg = 0;
        producer_semaphore.sem_num = S_PRODUCER;
        producer_semaphore.sem_op = -1;
        consumer_semaphore.sem_flg = 0;
        consumer_semaphore.sem_num = S_CONSUMER;
        consumer_semaphore.sem_op = 1;  
    
        while(1){
            if(semop(semaphore, &producer_semaphore, 1) == -1)
                print_error_exit(errno, "semop error producer, &producer_semaphore");
            
            time_t t = time(NULL);
            char *string_time = ctime(&t);
            int r =rand();

            memory->last_produced++;
            memory->array[memory->last_produced]=r;

            printf("PID:%d#TIME_STAMP%s#ADDED: %d#REMAINING_TASKS %d: ",getpid(), string_time, r, memory->last_produced+1);    
            usleep(1000000);
            if(semop(semaphore, &consumer_semaphore, 1) == -1)
                print_error_exit(errno, "semop error producer, &consumer_semaphore");
        }
    }else {
        struct sembuf producer_semaphore, consumer_semaphore;
        producer_semaphore.sem_flg = 0;
        producer_semaphore.sem_num = S_PRODUCER;
        producer_semaphore.sem_op = 1;
        consumer_semaphore.sem_flg = 0;
        consumer_semaphore.sem_num = S_CONSUMER;
        consumer_semaphore.sem_op = -1;   
    
        while(1){
            if(semop(semaphore, &consumer_semaphore, 1) == -1)
                print_error_exit(errno, "semop error producer, &consumer_semaphore");
            
            time_t t = time(NULL);
            char *string_time = ctime(&t);
            int r = memory->array[memory->last_produced];

            memory->last_produced--;
            if(is_prime(r))
                printf("PID:%d#TIME_STAMP%s#CHECKED NUMBER: %d - IS_PRIME #REMAINING_TASKS %d: ",getpid(), string_time, r, memory->last_produced+1);    
            else
                printf("PID:%d#TIME_STAMP%s#CHECKED NUMBER: %d - IS_NOT_PRIME #REMAINING_TASKS %d: ",getpid(), string_time, r, memory->last_produced+1);    
            usleep(1000000);
            if(semop(semaphore, &producer_semaphore, 1) == -1)
                print_error_exit(errno, "semop error producer, &producer_semaphore");
        }
    }



    return 0;
}
