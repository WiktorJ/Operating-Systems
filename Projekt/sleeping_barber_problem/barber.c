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


void* client_thread(void* arg);
void* barber_thread(void* arg);

void print_error(int error, char* message){
    puts(message);
    puts(strerror(error));
}

void print_error_exit(int error, char* message){
    puts(message);
    puts(strerror(error));
    exit(EXIT_FAILURE);
}

pthread_mutex_t draw_out;

sem_t barber_ready;
sem_t access_waiting_room_seats;
sem_t customer_ready;

int number_of_clients;
int number_of_seats;
int number_of_cuts;

pthread_t *clients;
pthread_t barber;

int number_of_free_seats;
int number_of_out_of_salon_seats;

struct bench *haircut_salon;
struct coordinates *haircut_chairs;
int *haircut_colors;
struct label *haircut_labels;

struct bench *barber_room;
struct coordinates *barber_chairs;
int *barber_colors;
struct label *barber_labels;

struct bench *waiting_room;
struct coordinates *waiting_chairs;
int *waiting_colors;
struct label *waiting_labels;

struct bench *out_of_salon_room;
struct coordinates *out_of_salon_chairs;
int *out_of_salon_colors;
struct label *out_of_salon_labels;

int main(int argc, char **argv){
    if(argc != 4){
        printf("usage: ./barber <number of clients> <number of waiting room seats> <number of hair cuts per client>");
        exit(EXIT_FAILURE);
    }

    number_of_clients = atoi(argv[1]);
    number_of_seats = atoi(argv[2]);
    number_of_cuts = atoi(argv[3]);
    number_of_free_seats = number_of_seats;
    number_of_out_of_salon_seats = number_of_clients;

    if( (pthread_mutex_init(&draw_out, NULL)) !=0 )
        print_error_exit(errno, "error in pthread_mutex_init");

    sem_init(&barber_ready, 0, 0);
    sem_init(&access_waiting_room_seats, 0, 1);
    sem_init(&customer_ready, 0, 0);

    int i;
    int r_id[number_of_clients];
    for(i = 0; i < number_of_clients; i++){
        r_id[i]=i;
    }

    haircut_salon= malloc(sizeof(struct bench));
    haircut_chairs = malloc(sizeof(struct coordinates) * 2);
    haircut_colors = malloc(sizeof(int) * 2);
    haircut_labels = malloc(sizeof(struct label) * 2);

    barber_room= malloc(sizeof(struct bench));
    barber_chairs = malloc(sizeof(struct coordinates));
    barber_colors = malloc(sizeof(int));
    barber_labels = malloc(sizeof(struct label));

    waiting_room= malloc(sizeof(struct bench));
    waiting_chairs = malloc(sizeof(struct coordinates) * number_of_seats);
    waiting_colors = malloc(sizeof(int) * number_of_seats);
    waiting_labels = malloc(sizeof(struct label) * number_of_seats);

    out_of_salon_room= malloc(sizeof(struct bench));
    out_of_salon_chairs = malloc(sizeof(struct coordinates) * number_of_out_of_salon_seats);
    out_of_salon_colors = malloc(sizeof(int) * number_of_out_of_salon_seats);
    out_of_salon_labels = malloc(sizeof(struct label) * number_of_out_of_salon_seats);

    for(i = 0; i < 2; i++){
        haircut_colors[i] = 6;
        haircut_labels[i].label = malloc(sizeof(char)* MAX_LABEL_SIZE);
        haircut_labels[i].label = " ";
    }

        barber_colors[0] = 6;
        barber_labels[0].label = malloc(sizeof(char)* MAX_LABEL_SIZE);
        barber_labels[0].label = " ";


    for(i = 0; i < number_of_seats; i++){
        waiting_colors[i] = 6;
        waiting_labels[i].label = malloc(sizeof(char)* MAX_LABEL_SIZE);
        waiting_labels[i].label = " ";
    }

    for(i = 0; i < number_of_out_of_salon_seats; i++){
        out_of_salon_colors[i] = 6;
        out_of_salon_labels[i].label = malloc(sizeof(char)* MAX_LABEL_SIZE);
        out_of_salon_labels[i].label = " ";
    }

    draw_bench(haircut_salon, LEFT, MIDDLE, HORIZONTAL, 2, haircut_chairs, "HAIRCUT SALON", haircut_labels);
    draw_bench(barber_room, LEFT, UP, HORIZONTAL, 1, barber_chairs, "BARBER CHAIR", barber_labels);
    draw_bench(waiting_room, LEFT, BOTTOM, HORIZONTAL, number_of_seats, waiting_chairs, "WAITING ROOM", waiting_labels);
    draw_bench(out_of_salon_room, RIGHT, UP, VERTICAL, number_of_out_of_salon_seats, out_of_salon_chairs, "OUT OF SALON", out_of_salon_labels);

    clients = malloc(sizeof(pthread_t) *number_of_clients);

    for(i = 0; i < number_of_clients; i++){
        if(pthread_create(&clients[i], NULL, client_thread, r_id + i) == -1)
            print_error(errno, "pthread_create error");
    }

    if(pthread_create(&barber, NULL, barber_thread, r_id) == -1)
        print_error(errno, "pthread_create error");


    for(i = 0; i < number_of_clients; i++){
        if(pthread_join(clients[i], NULL) == -1)
            print_error(errno, "error in pthread_join");
    }
    if(pthread_join(barber, NULL) == -1)
        print_error(errno, "error in pthread_join");

    return 0;
}



void* barber_thread(void* arg) {
    int i = 0;
    int id = *((int *) arg);
    srand(time(NULL));
    while (1) {
        initscr();
        start_color();

        if (pthread_mutex_lock(&draw_out) != 0)
            print_error(errno, "mutex_unlock error");
            draw_chair(barber_chairs[id], 1, "B", '%', id);
        if (pthread_mutex_unlock(&draw_out) != 0)
            print_error(errno, "mutex_unlock error");

        sem_wait(&customer_ready);

        if (pthread_mutex_lock(&draw_out) != 0)
            print_error(errno, "mutex_unlock error");
            draw_chair(barber_chairs[id], 1, " ", ' ', id);
            draw_chair(haircut_chairs[0], 1, "B", '%', id);
        if (pthread_mutex_unlock(&draw_out) != 0)
            print_error(errno, "mutex_unlock error");

        sem_wait(&access_waiting_room_seats);

        number_of_free_seats++;

        sem_post(&barber_ready);
        sem_post(&access_waiting_room_seats);
//        printf("barber is cutting hair: ");
        usleep(2000000);
        if (pthread_mutex_lock(&draw_out) != 0)
            print_error(errno, "mutex_unlock error");
        draw_chair(haircut_chairs[0], 1, " ", ' ', id);
        if (pthread_mutex_unlock(&draw_out) != 0)
            print_error(errno, "mutex_unlock error");

        endwin();
    }
    return NULL;
}


void* client_thread(void* arg) {
    int i = 0;
    int id = *((int *) arg);
    srand(time(NULL));
    while (1) {
        initscr();
        start_color();
        if (pthread_mutex_lock(&draw_out) != 0)
            print_error(errno, "mutex_unlock error");
            draw_chair(out_of_salon_chairs[id], 2, "C", '%', id);
        if (pthread_mutex_unlock(&draw_out) != 0)
            print_error(errno, "mutex_unlock error");
        usleep((rand()%3 +1) * 3000000);
        if (pthread_mutex_lock(&draw_out) != 0)
            print_error(errno, "mutex_unlock error");
        draw_chair(out_of_salon_chairs[id], 3, "C", '%', id);
        if (pthread_mutex_unlock(&draw_out) != 0)
            print_error(errno, "mutex_unlock error");

        sem_wait(&access_waiting_room_seats);
        if(number_of_free_seats > 0){
            number_of_free_seats--;

            int tmp = number_of_free_seats;
            if (pthread_mutex_lock(&draw_out) != 0)
                print_error(errno, "mutex_unlock error");
            draw_chair(out_of_salon_chairs[id], 2, " ", ' ', id);
            draw_chair(waiting_chairs[tmp], 2, "C", '%', id);
            if (pthread_mutex_unlock(&draw_out) != 0)
                print_error(errno, "mutex_unlock error");
            usleep((rand()%3 +1) * 500000);
            sem_post(&customer_ready);
                sem_post(&access_waiting_room_seats);
            sem_wait(&barber_ready);
            if (pthread_mutex_lock(&draw_out) != 0)
                print_error(errno, "mutex_unlock error");
            draw_chair(waiting_chairs[tmp], 2, " ", ' ', id);
            draw_chair(haircut_chairs[1], 2, "C", '%', id);
            if (pthread_mutex_unlock(&draw_out) != 0)
                print_error(errno, "mutex_unlock error");
//            printf("Client %d is having hair cut\n", id);
            usleep(2000000);
            if (pthread_mutex_lock(&draw_out) != 0)
                print_error(errno, "mutex_unlock error");
            draw_chair(haircut_chairs[1], 2, " ", ' ', id);
            if (pthread_mutex_unlock(&draw_out) != 0)
                print_error(errno, "mutex_unlock error");

        } else {
            sem_post(&access_waiting_room_seats);
//            printf("Client %d left without hair cut\n", id);
            usleep((rand()%3 +1) *1000000);
        }

        endwin();
    }
    return NULL;
}
