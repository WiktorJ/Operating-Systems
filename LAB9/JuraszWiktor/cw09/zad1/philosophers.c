#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
void* philosopher_thread(void* arg);
void print_error_exit(int error, char *message);
void print_error(int error, char *message);

#define SIZE 5


int portions;
pthread_t threads[SIZE];
pthread_mutex_t forks;
pthread_cond_t forks_cond[SIZE];
int taken[SIZE];
pthread_cond_t butler;
int how_many;
int main(int argc, char **argv){
    
    if(argc != 2)
        print_error_exit(errno, "usage ./philosophers <number of portion>");

    portions = atoi(argv[1]);
    
    //pthread_mutexattr_t attr;
    //if((errno = pthread_mutexattr_init(&attr)) > 0)
      //  print_error_exit(errno, "pthread_mutex_init()");
    //if((errno = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL)) > 0)
      //  print_error_exit(errno, "pthread_mutex_init()");
    
    if( (pthread_mutex_init(&forks, NULL)) !=0 )  
        print_error_exit(errno, "error in pthread_mutex_init");

    int i;
    int id_table[SIZE];
    if(pthread_cond_init(&butler,NULL) !=0)
            print_error_exit(errno, "cond_init error");
    for(i = 0; i < SIZE; i++){
        id_table[i] = i;
        if(pthread_cond_init(&forks_cond[i],NULL) !=0)
            print_error_exit(errno, "cond_init error");
        taken[i] = 0;
        how_many = 0;
    }


    
    for(i = 0; i < SIZE; i++){
        
        if(pthread_create(&threads[i], NULL, philosopher_thread, id_table + i) == -1);
    }
    
    for(i = 0; i < SIZE; i++){
        if(pthread_join(threads[i], NULL) == -1)
            print_error(errno, "error in pthread_join");
    }
    
    exit(EXIT_SUCCESS);
}


void print_error_exit(int error, char* message){
    puts(message);
    puts(strerror(error));
    exit(EXIT_FAILURE);
}


void print_error(int error, char* message){
    puts(message);
    puts(strerror(error));
}

void* philosopher_thread(void* arg){

    int i;
    int id = *((int*) arg);
    int left = id;
    int right = (id + 1)%SIZE;

    for(i = 0; i < portions; i++){
        usleep(1000);
        
        if(pthread_mutex_lock(&forks) == 0){
            if(how_many == 4){
                if(pthread_cond_wait(&butler, &forks) !=0)
                    print_error(errno,"pthread_cond error");            
            }
            how_many++;
        } else  
           print_error(errno, "mutex_lock error");

       
    
        if(taken[left] == 1){        
                if(pthread_cond_wait(&forks_cond[left], &forks) !=0)
                    print_error(errno,"pthread_cond error");            
        }
        taken[left] = 1;
        
        if(taken[right] == 1){        
                if(pthread_cond_wait(&forks_cond[right], &forks) !=0)
                    print_error(errno,"pthread_cond error");                
        }
        taken[right] = 1;
        
        if(pthread_mutex_unlock(&forks) != 0)
            print_error(errno,"mutex_unlock error");        
        

        printf("\nphilosopher %d is eating with forks %d and %d",id, left,right);
        usleep(5000);
        printf("\nphilosopher %d finished eating with forks %d and %d he will put them down in a moment",id,left,right);
        if(pthread_mutex_lock(&forks) == 0){ 
            
            taken[left] = 0;
            pthread_cond_signal(&forks_cond[left]);
            taken[right] = 0;
            pthread_cond_signal(&forks_cond[right]);         
            how_many--;
            
            if(how_many < 4){
                if(pthread_cond_signal(&butler) !=0)
                    print_error(errno,"pthread_cond error");            
            }
        } else 
            print_error(errno, "mutex_lock error");
        if(pthread_mutex_unlock(&forks) != 0)
            print_error(errno, "unlock error");
            
    }
    return NULL;

}
