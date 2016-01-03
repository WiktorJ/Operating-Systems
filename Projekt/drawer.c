//
// Created by wiktor on 05.06.15.
//

#include "drawer.h"
#include <ncurses.h>
#include <math.h>
#include <string.h>

int own_errno  = 0;

int draw_philosophers_table(struct coordinates center, struct table *structtable, struct coordinates *cords, struct coordinates *forks) {
    int size = 5;

    if(size > MAX_TABLE_SIZE){
        own_errno = 1;
        return -1;
    }

    int rows, cols, x, y;
    int table_size = 5*size;
    rows = 238;
    cols = 58;
    initscr();
    int r;
    erase();
    for(y = 0; y < cols; y++){
        for(x = 0; x < rows; x++){
            r = sqrt((center.x-x)*(center.x-x) +  4*(center.y-y)*(center.y-y));
            if(r > table_size || r < table_size -2){
                continue;
            }
            else if(r == table_size){
                mvaddch(y,x,'*');
            }
            else{
                mvaddch(y,x,'#');
            }
        }
    }

    structtable->table_center.x = center.x;
    structtable->table_center.y = center.y;

    int i;
    cords[0].x = 119;
    cords[0].y = 13;
    cords[1].x = 149;
    cords[1].y = 22;
    cords[2].x = 145;
    cords[2].y = 37;
    cords[3].x = 92;
    cords[3].y = 37;
    cords[4].x = 88;
    cords[4].y = 22;

    int fork_size = 6;
    forks[0].x = 106;
    forks[0].y = 19;
    forks[1].x = 132;
    forks[1].y = 19;
    forks[2].x = 134;
    forks[2].y = 28;
    forks[3].x = 119;
    forks[3].y = 34;
    forks[4].x = 98;
    forks[4].y = 28;




    int colors[size];
    for(i = 0; i< size + 1; i++){
        colors[i]=6;
    }
    refresh();
    start_color();
    draw_chairs(cords, colors, size, "P", '%');
    draw_forks(forks,  colors, fork_size, size);
    endwin(); /* Exit ncurses */

//    curs_set(0); /* hide text cursor (supposedly) */

//    curs_set(1); /* unhide cursor */

    return 0;
}


    int draw_chairs(struct coordinates *cords, int *colors, int size, char *label, char border){
    int i;
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_WHITE, COLOR_BLACK);
    for(i = 0; i<size; i++){
        draw_chair(cords[i], colors[i], label, border, i);
    }

    return 0;
}

int draw_chair(struct coordinates cords, int color, char *label, char border, int i){
    char str[30];
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(color));
    sprintf(str, "%d", i+1);
    strcat(str, label);
    mvaddstr(cords.y,cords.x, str);
    mvaddch(cords.y-2, cords.x-3, border);
    mvaddch(cords.y-2, cords.x-2, border);
    mvaddch(cords.y-2, cords.x-1, border);
    mvaddch(cords.y-2, cords.x, border);
    mvaddch(cords.y-2, cords.x+1, border);
    mvaddch(cords.y-2, cords.x+2, border);
    mvaddch(cords.y-2, cords.x+3, border);
    mvaddch(cords.y-2, cords.x+4, border);
    mvaddch(cords.y+2, cords.x-3, border);
    mvaddch(cords.y+2, cords.x-2, border);
    mvaddch(cords.y+2, cords.x-1, border);
    mvaddch(cords.y+2, cords.x, border);
    mvaddch(cords.y+2, cords.x+1, border);
    mvaddch(cords.y+2, cords.x+2, border);
    mvaddch(cords.y+2, cords.x+3, border);
    mvaddch(cords.y+2, cords.x+4, border);
    mvaddch(cords.y-1, cords.x-4, border);
    mvaddch(cords.y-1, cords.x+5, border);
    mvaddch(cords.y, cords.x-5, border);
    mvaddch(cords.y+1, cords.x+5, border);
    mvaddch(cords.y+1, cords.x-4, border);
    mvaddch(cords.y, cords.x+6, border);
    attroff(COLOR_PAIR(color));
    refresh();
    return 0;

}


int draw_forks(struct coordinates *forks, int *colors, int fork_size, int size) {
    int j,i;

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_WHITE, COLOR_BLACK);


    for(i = 0; i<2; i++){
        attron(COLOR_PAIR(colors[i]));
        for(j = 0; j < fork_size; j++) {
            if(i == 0)
                mvaddch(forks[i].y + j, forks[i].x + j, '@');

            else
                mvaddch(forks[i].y + j, forks[i].x - j, '@');
        }
        attroff(COLOR_PAIR(colors[i]));
    }
    for(i = 2; i<size; i++){
        attron(COLOR_PAIR(colors[i]));
        for(j = 0; j < fork_size; j++){
            if(i == 2 || i == 4)
                mvaddch(forks[i].y, forks[i].x+j, '@');
            else
                mvaddch(forks[i].y+j, forks[i].x, '@');
        }
        attroff(COLOR_PAIR(colors[i]));
    }
    refresh();
    return 0;
}

int draw_bench(struct bench *bench, int horizontal_alignment, int vertical_alignment, int orientation, int size, struct coordinates *chairs, char *desc, struct label *labels) {
    if(size > MAX_TABLE_SIZE){
        own_errno = 1;
        return -1;
    }
    initscr();
    int desc_len = strlen(desc);
    if(orientation == HORIZONTAL){

        bench->left_bottom_corner[0].x = (horizontal_alignment == LEFT) ? 1 : 238 - size* BOX_H_SIZE - 1;
//        bench->left_bottom_corner[0].y = (vertical_alignment == BOTTOM ) ? 58 - BOX_V_SIZE - 1 : 1;
        if(vertical_alignment == BOTTOM) {
            bench->left_bottom_corner[0].y = 58 - BOX_V_SIZE - 1;
        }
        else if(vertical_alignment == UP) {
            bench->left_bottom_corner[0].y = 1;
        }
        else {
            bench->left_bottom_corner[0].y = 26;
        }

        int middle = size * BOX_H_SIZE/2;
        mvaddstr(bench->left_bottom_corner[0].y - 1, bench->left_bottom_corner[0].x + middle - (desc_len/2), desc);

        int i;
        for(i = 0; i < size; i++){
            draw_box(bench->left_bottom_corner[i]);
            chairs[i].x = bench->left_bottom_corner[i].x + BOX_H_SIZE / 2;
            chairs[i].y = bench->left_bottom_corner[i].y + 3;
            labels[i].cords.x = bench->left_bottom_corner[i].x + BOX_H_SIZE/2 - strlen(labels->label)/2;
            labels[i].cords.y = bench->left_bottom_corner[i].y  + BOX_V_SIZE -1;
            draw_label(labels[i]);
            if(i < size -1) {
                bench->left_bottom_corner[i + 1].x = bench->left_bottom_corner[i].x + BOX_H_SIZE;
                bench->left_bottom_corner[i + 1].y = bench->left_bottom_corner[i].y;

            }
        }
    } else {
        bench->left_bottom_corner[0].x = (horizontal_alignment == LEFT) ? 1 : 238 - BOX_H_SIZE - 2;
        bench->left_bottom_corner[0].y = (vertical_alignment == BOTTOM ) ? 58 - size * BOX_V_SIZE - 1 : 1;

        int middle = size * BOX_V_SIZE/2;
        int i;
        for(i = 0; i<desc_len; i++){
            mvaddch(bench->left_bottom_corner[0].y + middle - (desc_len/2) + i, bench->left_bottom_corner[0].x  + BOX_H_SIZE + 1, desc[i]);
        }
        for(i = 0; i < size; i++){
            draw_box(bench->left_bottom_corner[i]);
            chairs[i].x = bench->left_bottom_corner[i].x + BOX_H_SIZE / 2;
            chairs[i].y = bench->left_bottom_corner[i].y + 3;
            labels[i].cords.x = bench->left_bottom_corner[i].x + BOX_H_SIZE/2 - strlen(labels->label)/2;
            labels[i].cords.y = bench->left_bottom_corner[i].y + BOX_V_SIZE -1;
            draw_label(labels[i]);
            if(i < size -1) {
                bench->left_bottom_corner[i + 1].x = bench->left_bottom_corner[i].x;
                bench->left_bottom_corner[i + 1].y = bench->left_bottom_corner[i].y + BOX_V_SIZE;

            }
        }
    }
    refresh();
    getch();
    endwin();

    return 0;
}

void draw_label(struct label label) {
    mvaddstr(label.cords.y, label.cords.x, label.label);
}

void draw_box(struct coordinates coordinates) {
    int i;
    for(i = coordinates.x; i < coordinates.x + BOX_H_SIZE; i++){
        mvaddch(coordinates.y, i, '-');
    }

    for(i = coordinates.x; i < coordinates.x + BOX_H_SIZE; i++){
        mvaddch(coordinates.y + BOX_V_SIZE, i, '-');
    }

    for(i = coordinates.y; i < coordinates.y + BOX_V_SIZE; i++){
        mvaddch(i , coordinates.x, '|');
    }

    for(i = coordinates.y; i < coordinates.y + BOX_V_SIZE; i++){
        mvaddch(i , coordinates.x + BOX_H_SIZE, '|');
    }

}
