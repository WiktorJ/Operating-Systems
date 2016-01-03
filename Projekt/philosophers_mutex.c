//
// Created by wiktor on 05.06.15.
//

#include <stdio.h>
#include <stdlib.h>
#include "drawer.h"

int main(int argc, char **argv){
    int size = 7;
//    struct table *table = malloc(sizeof(table));
//    struct coordinates cords;
    struct coordinates *chairs = malloc(sizeof(struct coordinates) * size);
//    struct coordinates *forks = malloc(sizeof(struct coordinates) * 5);
    struct bench *bench = malloc(sizeof(bench) * size);
    struct label *label = malloc(sizeof(label) * size);

//    cords.x = 119;
//    cords.y = 28;
//    if(draw_philosophers_table(cords, table,  chairs, forks) == -1){
//        printf("error: %d: ", own_errno);
//        exit(EXIT_FAILURE);
//    }
    int i;
    int colors[size];
    for(i = 0; i < size; i++){
        colors[i]=6;
        label[i].label = malloc(sizeof(char)* MAX_LABEL_SIZE);
        label[i].label = "123";
    }
    draw_bench(bench, RIGHT, UP, HORIZONTAL, size, chairs, "some description", label);
    draw_chairs(chairs, colors ,size, "G", '%');
    getchar();
    return 0;
}