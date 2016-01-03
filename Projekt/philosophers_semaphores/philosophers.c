//
// Created by wiktor on 16.06.15.
//
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
#include "../drawer.h"
#include <pthread.h>
#include <ncurses.h>

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

sem_t forks[SIZE];
sem_t butler;

int taken[SIZE];
int how_many;

pthread_mutex_t draw_out;

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
    if(sem_destroy(&butler) == -1)
        print_error(errno, "sem_destroy pmutex error");
    int i;
    for(i = 0; i < SIZE; i++){
        if(sem_destroy(&forks[i]) == -1)
            print_error(errno, "sem_destroy cmutex error");
    }
}

int main(int argc, char **argv){

    if(argc != 2)
        print_error_exit(errno, "usage ./philosophers <number of portion>");

    portions = atoi(argv[1]);

    int i;
    int id_table[SIZE];

    for(i = 0; i < SIZE; i++){
        id_table[i] = i;
        taken[i] = 0;
        how_many = 0;
        sem_init(&forks[i], 0, 1);
    }
    sem_init(&butler, 0, SIZE - 1);

    if( (pthread_mutex_init(&draw_out, NULL)) !=0 )
        print_error_exit(errno, "error in pthread_mutex_init");

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


void* philosopher_thread(void* arg){

    int i;
    int id = *((int*) arg);
    int left = id;
    int right = (id + 1)%SIZE;

    for(i = 0; i < portions; i++){
        initscr();
        start_color();
//        printf("philosopher %d is thinking\n", left);
        if(pthread_mutex_lock(&draw_out) != 0)
            print_error(errno,"mutex_unlock error");
            chairs_colors[left] = 6;
            draw_chair(chairs[id], chairs_colors[left], "P", ' ', left);
        if(pthread_mutex_unlock(&draw_out) != 0)
            print_error(errno,"mutex_unlock error");
        usleep((rand()%3 +1) * 2000000);

        if(pthread_mutex_lock(&draw_out) != 0)
            print_error(errno,"mutex_unlock error");
        chairs_colors[left] = 6;
        draw_chair(chairs[id], chairs_colors[left], "P", '%', left);
        if(pthread_mutex_unlock(&draw_out) != 0)
            print_error(errno,"mutex_unlock error");
        sem_wait(&butler);


        if(pthread_mutex_lock(&draw_out) != 0)
            print_error(errno,"mutex_unlock error");
        chairs_colors[left] = 6;
        draw_chair(chairs[id], chairs_colors[left], "P", '%', left);
        if(pthread_mutex_unlock(&draw_out) != 0)
            print_error(errno,"mutex_unlock error");

        sem_wait(&forks[left]);

        if(pthread_mutex_lock(&draw_out) != 0)
            print_error(errno,"mutex_unlock error");
            forks_colors[left] = left+1;
            draw_forks(forks_cords, forks_colors, fork_size, SIZE);
        if(pthread_mutex_unlock(&draw_out) != 0)
            print_error(errno,"mutex_unlock error");


        sem_wait(&forks[right]);
        if(pthread_mutex_lock(&draw_out) != 0)
            print_error(errno,"mutex_unlock error");
            forks_colors[right] = left+1;
             draw_forks(forks_cords, forks_colors, fork_size, SIZE);
            chairs_colors[left] = left+1;
            draw_chair(chairs[left], chairs_colors[left], "P", '%', left);
        if(pthread_mutex_unlock(&draw_out) != 0)
            print_error(errno,"mutex_unlock error");

//        printf("philosopher %d is eating with forks: %d and %d\n", left, left, right);
        usleep((rand()%3 +1) * 3000000);
        if(pthread_mutex_lock(&draw_out) != 0)
            print_error(errno,"mutex_unlock error");
        forks_colors[left] = 6;
        forks_colors[right] = 6;
        chairs_colors[left] = 6;
        draw_forks(forks_cords, forks_colors, fork_size, SIZE);
        draw_chair(chairs[left], chairs_colors[left], "P", '%', left);
        if(pthread_mutex_unlock(&draw_out) != 0)
            print_error(errno,"mutex_unlock error");
        sem_post(&forks[right]);
        sem_post(&forks[left]);
//        printf("philosopher %d put down forks: %d and %d\n", left, left, right);
        sem_post(&butler);

        endwin();
    }
    return NULL;

}
