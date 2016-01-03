#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ncurses.h>
#include "../drawer.h"
void* philosopher_thread(void* arg);
void print_error_exit(int error, char *message);
void print_error(int error, char *message);

#define SIZE 5
int fork_size = 6;
struct coordinates cords;
struct table *table;
struct coordinates *chairs;
struct coordinates *forks_cords;
int forks_colors[SIZE];
int chairs_colors[SIZE];

int portions;
pthread_t threads[SIZE];
pthread_mutex_t forks;
pthread_mutex_t draw_out;
pthread_cond_t forks_cond[SIZE];
int taken[SIZE];
pthread_cond_t butler;
int how_many;
int main(int argc, char **argv){
    
    if(argc != 2)
        print_error_exit(errno, "usage ./philosophers <number of portion>");

    portions = atoi(argv[1]);

    if( (pthread_mutex_init(&forks, NULL)) !=0 )
        print_error_exit(errno, "error in pthread_mutex_init");
    if( (pthread_mutex_init(&draw_out, NULL)) !=0 )
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
    table = malloc(sizeof(table));
    chairs = malloc(sizeof(struct coordinates) * SIZE);
    forks_cords = malloc(sizeof(struct coordinates) * SIZE);
    cords.x = 119;
    cords.y = 28;
    if(draw_philosophers_table(cords, table,  chairs, forks_cords) == -1){
        printf("error: %d: ", own_errno);
        exit(EXIT_FAILURE);
    }
    for(i = 0; i< SIZE + 1; i++){
        forks_colors[i]=6;
        chairs_colors[i]=6;
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
        usleep((rand()%3 +1) * 2000000);
        initscr();
        start_color();

        if(pthread_mutex_lock(&forks) == 0){
            chairs_colors[left] = 6;
            draw_chair(chairs[left], chairs_colors[left], "P", '!', left);
            if(how_many == 4){
                if(pthread_cond_wait(&butler, &forks) !=0)
                    print_error(errno,"pthread_cond error");            
            }
            how_many++;
            draw_chair(chairs[left], chairs_colors[left], "P", '%', left);
        } else  
           print_error(errno, "mutex_lock error");


        if(taken[left] == 1){
                if(pthread_cond_wait(&forks_cond[left], &forks) !=0)
                    print_error(errno,"pthread_cond error");            
        }
        forks_colors[left] = left+1;
        taken[left] = 1;

        draw_forks(forks_cords, forks_colors, fork_size, SIZE);
        if(taken[right] == 1){
                if(pthread_cond_wait(&forks_cond[right], &forks) !=0)
                    print_error(errno,"pthread_cond error");                
        }
        forks_colors[right] = left+1;
        taken[right] = 1;

        draw_forks(forks_cords, forks_colors, fork_size, SIZE);




        chairs_colors[left] = left+1;
        draw_chair(chairs[left], chairs_colors[left], "P", '%', left);
        if(pthread_mutex_unlock(&forks) != 0) {
            print_error(errno,"mutex_unlock error");
        }

        usleep((rand()%3 +1 ) * 3000000);
        if(pthread_mutex_lock(&forks) == 0){
            forks_colors[left] = 6;
            forks_colors[right] = 6;
            chairs_colors[left] = 6;
            draw_forks(forks_cords, forks_colors, fork_size, SIZE);

            draw_chair(chairs[left], chairs_colors[left], "P", ' ', left);
            endwin();

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
