//
// Created by wiktor on 14.06.15.
//

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

int number_of_readers;
int number_of_writers;

pthread_t *writers;
pthread_t *readers;

struct bench *reading_room;
struct coordinates *reading_chairs;
int *reading_colors;
struct label *reading_labels;

struct bench *condition_writers;
struct coordinates *writers_chairs;
int *writers_colors;
struct label *writers_labels;

struct bench *condition_readers;
struct coordinates *readers_chairs;
int *readers_colors;
struct label *readers_labels;

int *positions;

pthread_mutex_t reading_room_mutex;
pthread_mutex_t draw_out;

pthread_cond_t readers_cond;
pthread_cond_t writers_cond;

int is_reading, is_writing;
int readers_cond_waiting;
int writers_cond_waiting;

int main(int argc, char **argv){

    if(argc != 3)
        print_error_exit(errno,"usage ./writer <number of readers> <number of writers> ");

    number_of_readers = atoi(argv[1]);
    number_of_writers = atoi(argv[2]);
    positions = malloc(sizeof(int)*number_of_readers);
    int i;
    int w_id[number_of_writers];
    int r_id[number_of_readers];
    for(i = 0; i < number_of_readers; i++){
        r_id[i]=i;
        positions[i]=0;
    }
    for(i = 0; i < number_of_writers; i++){
        w_id[i]=i;
        positions[i + number_of_readers]=0;
    }

    writers_cond_waiting = readers_cond_waiting = is_reading = is_writing = 0;


    if( (pthread_mutex_init(&reading_room_mutex, NULL)) !=0 )
        print_error_exit(errno, "error in pthread_mutex_init");
    if( (pthread_mutex_init(&draw_out, NULL)) !=0 )
        print_error_exit(errno, "error in pthread_mutex_init");

    if(pthread_cond_init(&readers_cond, NULL) !=0)
        print_error_exit(errno, "cond_init error");
    if(pthread_cond_init(&writers_cond, NULL) !=0)
        print_error_exit(errno, "cond_init error");



    reading_room = malloc(sizeof(struct bench));
    reading_chairs = malloc(sizeof(struct coordinates) * (number_of_readers + number_of_writers));
    reading_colors = malloc(sizeof(int) * (number_of_readers + number_of_writers));
    reading_labels = malloc(sizeof(struct label) * (number_of_readers + number_of_writers));

    condition_writers = malloc(sizeof(struct bench));
    writers_chairs = malloc(sizeof(struct coordinates) * (number_of_writers));
    writers_colors = malloc(sizeof(int) * (number_of_writers));
    writers_labels = malloc(sizeof(struct label) * (number_of_writers));

    condition_readers = malloc(sizeof(struct bench));
    readers_chairs = malloc(sizeof(struct coordinates) * (number_of_readers));
    readers_colors = malloc(sizeof(int) * (number_of_readers));
    readers_labels = malloc(sizeof(struct label) * (number_of_readers));


    for(i = 0; i < number_of_readers + number_of_writers; i++){
        reading_colors[i] = 6;
        reading_labels[i].label = malloc(sizeof(char)* MAX_LABEL_SIZE);
        reading_labels[i].label = " ";
    }


    for(i = 0; i < number_of_writers; i++){
        writers_colors[i] = 6;
        writers_labels[i].label = malloc(sizeof(char)* MAX_LABEL_SIZE);
        writers_labels[i].label = " ";
    }


    for(i = 0; i < number_of_readers + number_of_writers; i++){
        readers_colors[i] = 6;
        readers_labels[i].label = malloc(sizeof(char)* MAX_LABEL_SIZE);
        readers_labels[i].label = " ";
    }

    draw_bench(reading_room, LEFT, MIDDLE, HORIZONTAL, number_of_readers + number_of_writers, reading_chairs, "READING ROOM", reading_labels);
    draw_bench(condition_writers, LEFT, BOTTOM, HORIZONTAL, number_of_writers, writers_chairs, "WRITERS CONDITION", writers_labels);
    draw_bench(condition_readers, LEFT, UP, HORIZONTAL, number_of_readers, readers_chairs, "READERS CONDITION", readers_labels);

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

void* reader_thread(void* arg) {
    int i = 0;
    int id = *((int *) arg);
    srand(time(NULL));
    int curr_post = id;
    positions[id] = 1;
    while (1) {
        initscr();
        start_color();
        if(pthread_mutex_lock(&reading_room_mutex) == 0){
            if(is_writing > 0 || writers_cond_waiting > 0){
                readers_cond_waiting++;
                if (pthread_mutex_lock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");
                draw_chair(readers_chairs[id], 2, "R", '%', id);
                if (pthread_mutex_unlock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");

                if(pthread_cond_wait(&readers_cond, &reading_room_mutex) !=0)
                    print_error(errno,"pthread_cond error");
//                usleep(1000000);
                if (pthread_mutex_lock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");
                draw_chair(readers_chairs[id], 2, " ", ' ', id);
                if (pthread_mutex_unlock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");

                readers_cond_waiting--;
            }
            is_reading++;
        } else
            print_error(errno, "mutex_lock error");


        if(pthread_mutex_unlock(&reading_room_mutex) != 0) {
            print_error(errno,"mutex_unlock error");
        }

//        printf("Reader %d is reading\n", id);
        if (pthread_mutex_lock(&draw_out) != 0)
            print_error(errno, "mutex_unlock error");
        draw_chair(reading_chairs[curr_post], 2, "R", '%', id);
        if (pthread_mutex_unlock(&draw_out) != 0)
            print_error(errno, "mutex_unlock error");

        usleep((rand()%3+1)*2000000);

        if(pthread_mutex_lock(&reading_room_mutex) == 0){
            if (pthread_mutex_lock(&draw_out) != 0)
                print_error(errno, "mutex_unlock error");
            draw_chair(reading_chairs[curr_post], 2, " ", ' ', id);
            if (pthread_mutex_unlock(&draw_out) != 0)
                print_error(errno, "mutex_unlock error");
            positions[curr_post] = 0;
            curr_post = (curr_post + rand()%3) % (number_of_readers + number_of_writers);
            while(positions[curr_post] == 1){
                curr_post = (curr_post + 1) % (number_of_readers + number_of_writers);
            }
            positions[curr_post] = 1;
            is_reading--;
            if(is_reading == 0) {
                if (pthread_cond_signal(&writers_cond) != 0)
                    print_error(errno, "pthread_cond error");
            }
        } else
            print_error(errno, "mutex_lock error");

//        printf("Reader %d finished reading\n", id);

        if(pthread_mutex_unlock(&reading_room_mutex) != 0) {
            print_error(errno,"mutex_unlock error");
        }

//        endwin();
    }

}


void* writer_thread(void* arg){
    int i = 0;
    int id = *((int*) arg) + number_of_readers;
    srand(time(NULL));
    int amount = number_of_readers + number_of_writers;
    while(1){
        usleep(1000000);
        initscr();
        start_color();
        if(pthread_mutex_lock(&reading_room_mutex) == 0){
            if(is_writing + is_reading > 0){
                writers_cond_waiting++;
                if (pthread_mutex_lock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");
                draw_chair(writers_chairs[id-number_of_readers], 1, "R", '%', id);
                if (pthread_mutex_unlock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");

                if(pthread_cond_wait(&writers_cond, &reading_room_mutex) !=0)
                    print_error(errno,"pthread_cond error");
//                usleep(1000000);
                if (pthread_mutex_lock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");
                draw_chair(writers_chairs[id-number_of_readers], 1, " ", ' ', id);
                if (pthread_mutex_unlock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");

                writers_cond_waiting--;
            }
            is_writing = 1;
        } else
            print_error(errno, "mutex_lock error");



        if(pthread_mutex_unlock(&reading_room_mutex) != 0) {
            print_error(errno,"mutex_unlock error");
        }

//        printf("Writer %d is writing\n", id);
//        usleep(1000000);
        for (i = 0; i < amount; i++) {
            if (pthread_mutex_lock(&draw_out) != 0)
                print_error(errno, "mutex_unlock error");
            draw_chair(reading_chairs[i], 1, "W", '%', id);
            if (pthread_mutex_unlock(&draw_out) != 0)
                print_error(errno, "mutex_unlock error");
            usleep(1000000);
        }

        if(pthread_mutex_lock(&reading_room_mutex) == 0){

            for (i = 0; i < amount; i++) {

                if (pthread_mutex_lock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");

                draw_chair(reading_chairs[i], 1, " ", ' ', id);

                if (pthread_mutex_unlock(&draw_out) != 0)
                    print_error(errno, "mutex_unlock error");
            }
            is_writing = 0;

//            printf("Writer %d finished writing\n", id);

            if(readers_cond_waiting == 0) {
                if (pthread_cond_signal(&writers_cond) != 0)
                    print_error(errno, "pthread_cond error");
            } else {
                if (pthread_cond_broadcast(&readers_cond) != 0)
                    print_error(errno, "pthread_cond error");
            }

        }
        if(pthread_mutex_unlock(&reading_room_mutex) != 0) {
            print_error(errno,"mutex_unlock error");
        }


//        endwin();
    }

}
