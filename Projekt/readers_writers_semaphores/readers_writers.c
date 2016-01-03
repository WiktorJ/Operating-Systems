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
#define S_KEY 3
#define SIZE 20
void print_error(int error, char* message){
    puts(message);
    puts(strerror(error));
}

void print_error_exit(int error, char* message){
    puts(message);
    puts(strerror(error));
    exit(EXIT_FAILURE);
}
void * reader_thread(void * arg);
void * writer_thread(void * arg);
sem_t binary;
sem_t free_sem;
int creator;
int number_of_readers;
int number_of_writers;
key_t semaphore_key;
char *key_path = ".";
int resources[SIZE];
pthread_t *writers;
pthread_t *readers;
pthread_mutex_t draw_out;
pthread_mutex_t draw_sem;
pthread_mutex_t draw_read;

int *positions;

struct bench *reading_room;
struct coordinates *reading_chairs;
int *reading_colors;
struct label *reading_labels;

struct bench *sem_waiting;
struct coordinates *sem_chairs;
int *sem_colors;
struct label *sem_labels;

struct bench *out_waiting;
struct coordinates *out_chairs;
int *out_colors;
struct label *out_labels;


void atexit_function(){
    if(sem_destroy(&binary) == -1)
        print_error(errno, "sem_destroy pmutex error");

    if(sem_destroy(&free_sem) == -1)
        print_error(errno, "sem_destroy cmutex error");
}

int main(int argc, char** argv){
    if(argc != 3)
        print_error_exit(errno,"usage ./writer <number of readers> <number of writers> ");

    number_of_readers = atoi(argv[1]);
    number_of_writers = atoi(argv[2]);
    positions = malloc(sizeof(int)*number_of_readers);
    int i;
    atexit(atexit_function);
    int w_id[number_of_writers];
    int r_id[number_of_readers];
    for(i = 0; i < number_of_readers; i++){
        r_id[i]=i;
        positions[i]=0;
    }
    for(i = 0; i < number_of_writers; i++){
        w_id[i]=i;
    }

    if( (pthread_mutex_init(&draw_out, NULL)) !=0 )
        print_error_exit(errno, "error in pthread_mutex_init");
    if( (pthread_mutex_init(&draw_sem, NULL)) !=0 )
        print_error_exit(errno, "error in pthread_mutex_init");
    if( (pthread_mutex_init(&draw_read, NULL)) !=0 )
        print_error_exit(errno, "error in pthread_mutex_init");


    sem_init(&binary, 0, 1);
    sem_init(&free_sem, 0,number_of_readers);
    for(i = 0; i < number_of_readers + number_of_writers; i++)
        resources[i] = i/2;

    reading_room = malloc(sizeof(struct bench));
    sem_waiting = malloc(sizeof(struct bench));
    out_waiting = malloc(sizeof(struct bench));

    reading_chairs = malloc(sizeof(struct coordinates) * (number_of_readers + number_of_writers));
    sem_chairs = malloc(sizeof(struct coordinates) * (number_of_readers + number_of_writers));
    out_chairs = malloc(sizeof(struct coordinates) * (number_of_readers + number_of_writers));

    reading_colors = malloc(sizeof(int) * (number_of_readers + number_of_writers));
    sem_colors = malloc(sizeof(int) * (number_of_readers + number_of_writers));
    out_colors = malloc(sizeof(int) * (number_of_readers + number_of_writers));

    reading_labels = malloc(sizeof(struct label) * (number_of_readers + number_of_writers));
    sem_labels = malloc(sizeof(struct label) * (number_of_readers + number_of_writers));
    out_labels = malloc(sizeof(struct label) * (number_of_readers + number_of_writers));


    for(i = 0; i < number_of_readers + number_of_writers; i++){
        reading_colors[i] = 6;
        sem_colors[i] = 6;
        out_colors[i] = 6;
        reading_labels[i].label = malloc(sizeof(char)* MAX_LABEL_SIZE);
        sem_labels[i].label = malloc(sizeof(char)* MAX_LABEL_SIZE);
        out_labels[i].label = malloc(sizeof(char)* MAX_LABEL_SIZE);
        reading_labels[i].label = " ";
        sem_labels[i].label = " ";
        out_labels[i].label = " ";
    }

    draw_bench(reading_room, LEFT, MIDDLE, HORIZONTAL, number_of_readers + number_of_writers, reading_chairs, "READING ROOM", reading_labels);
    draw_bench(sem_waiting, LEFT, BOTTOM, HORIZONTAL, number_of_readers, sem_chairs, "HAVE SEMAPHORE", sem_labels);
    draw_bench(out_waiting, RIGHT, UP, VERTICAL, number_of_readers + number_of_writers, out_chairs, "WAITING", out_labels);

    writers = malloc(sizeof(pthread_t) *number_of_writers);
    readers = malloc(sizeof(pthread_t) *number_of_readers);

    for(i = 0; i < number_of_readers; i++){
        if(pthread_create(&readers[i], NULL, reader_thread, r_id + i) == -1);
    }
    for(i = 0; i < number_of_writers; i++){
        if(pthread_create(&writers[i], NULL, writer_thread, w_id + i) == -1);
    }
    
    for(i = 0; i < number_of_readers; i++){
        if(pthread_join(readers[i], NULL) == -1)
            print_error(errno, "error in pthread_join");
    }
    for(i = 0; i < number_of_writers; i++){
        if(pthread_join(writers[i], NULL) == -1)
            print_error(errno, "error in pthread_join");
    }

    return 0;
}
void* reader_thread(void* arg){
    int i = 0;
    int id = *((int*) arg);
    srand(time(NULL));

    while(1){
        initscr();
        start_color();

//        usleep(2000000);

        int tmp;

        for(i = 0; i < number_of_readers + number_of_writers; i += rand()%2+1){
            if(pthread_mutex_lock(&draw_out) != 0)
                print_error(errno,"mutex_unlock error");
            draw_chair(out_chairs[id], 2, "R", '%', id);
            if(pthread_mutex_unlock(&draw_out) != 0)
                print_error(errno,"mutex_unlock error");
            usleep(2000000);

            if(pthread_mutex_lock(&draw_read) != 0)
                print_error(errno,"mutex_unlock error");

                sem_wait(&free_sem);
                int j;
                for(j = 0; j < number_of_readers; j++) {
                    if (positions[j] == 0) {
                        positions[j] = 1;
                        tmp = j;
                        break;
                        }
                    }

            if(pthread_mutex_unlock(&draw_read) != 0)
                print_error(errno,"mutex_unlock error");


            if(pthread_mutex_lock(&draw_out) != 0)
                print_error(errno,"mutex_unlock error");
            draw_chair(out_chairs[id], 2, " ", ' ', id);
            draw_chair(sem_chairs[tmp], 2, "R", '%', id);
            if(pthread_mutex_unlock(&draw_out) != 0) {
                print_error(errno,"mutex_unlock error");
            }

            if(pthread_mutex_lock(&draw_out) != 0)
                print_error(errno,"mutex_unlock error");

            draw_chair(reading_chairs[i], 2, "R", '%', id);

            if(pthread_mutex_unlock(&draw_out) != 0)
                print_error(errno,"mutex_unlock error");


            usleep(2000000);

            if(pthread_mutex_lock(&draw_out) != 0)
                print_error(errno,"mutex_unlock error");

            draw_chair(reading_chairs[i], 2, " ", ' ', id);
            draw_chair(sem_chairs[tmp], 2, " ", ' ', id);
            if(pthread_mutex_unlock(&draw_out) != 0)
                print_error(errno,"mutex_unlock error");

            positions[tmp] = 0;
            sem_post(&free_sem);
        }
        endwin();
    }

}

void* writer_thread(void* arg){
    int i = 0;
    int id = *((int*) arg) + number_of_readers;
    srand(time(NULL));
    while(1){
        initscr();
        start_color();
        if(pthread_mutex_lock(&draw_out) != 0)
            print_error(errno,"mutex_unlock error");
        draw_chair(out_chairs[id], 1, "W", '%', id);
        if(pthread_mutex_unlock(&draw_out) != 0)
            print_error(errno,"mutex_unlock error");
        usleep(2000000);

        int amount = number_of_readers + number_of_writers;
        sem_wait(&binary);
        for(i = 0; i <number_of_readers; i++) {
            sem_wait(&free_sem);
            int j,tmp;
            for(j = 0; j < number_of_readers; j++) {
                if (positions[j] == 0) {
                    positions[j] = 1;
                    tmp = j;
                    break;
                }
            }


            if (pthread_mutex_lock(&draw_out) != 0) {
                print_error(errno, "mutex_unlock error");
            }
            draw_chair(out_chairs[id], 1, " ", ' ', id);
            draw_chair(sem_chairs[tmp], 1, "W", '%', id);
            if (pthread_mutex_unlock(&draw_out) != 0)
                print_error(errno, "mutex_unlock error");
            usleep(2000000);
        }
            for (i = 0; i < amount; i++) {
                if (pthread_mutex_lock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");
                draw_chair(reading_chairs[i], 1, "W", '%', id);
                if (pthread_mutex_unlock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");
//                    int r = rand() % 10;
//                    int p = rand() % (number_of_readers + number_of_writers);
//                    resources[p] = r;
                    usleep(500000);

            }

            for (i = 0; i < amount; i++) {

                if (pthread_mutex_lock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");

                draw_chair(reading_chairs[i], 1, " ", ' ', id);

                if (pthread_mutex_unlock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");
            }

            for (i = 0; i < number_of_readers; i++) {
                positions[i] = 0;
                sem_post(&free_sem);

                if (pthread_mutex_lock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");

                draw_chair(sem_chairs[i], 1, " ", ' ', id);

                if (pthread_mutex_unlock(&draw_out) != 0)
                print_error(errno, "mutex_unlock error");
            }

            sem_post(&binary);
        endwin();
    };
}
