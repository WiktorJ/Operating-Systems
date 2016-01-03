//
// Created by wiktor on 05.06.15.
//

#ifndef PROJEKT_DRAWER_H
#define PROJEKT_DRAWER_H
#define MAX_TABLE_SIZE 8
#define MAX_LABEL_SIZE 10
#define BOTTOM 1
#define UP 2
#define MIDDLE 3
#define LEFT 1
#define RIGHT 2
#define HORIZONTAL 1
#define VERTICAL 2
#define BOX_H_SIZE 17
#define BOX_V_SIZE 7
extern int own_errno;
struct coordinates{
    int x;
    int y;
};
struct table{
    struct coordinates table_center;
};

struct bench{
    struct coordinates left_bottom_corner[MAX_TABLE_SIZE];
    int taken[MAX_TABLE_SIZE];
};

struct label{
    struct coordinates cords;
    char *label;
};
void draw_label(struct label label);
int draw_bench(struct bench *bench, int horizontal_alignment, int vertical_alignment, int orientation, int size, struct coordinates *chairs, char* desc, struct label *label);
int draw_forks(struct coordinates *forks, int *colors, int fork_size, int size);
int draw_chairs(struct coordinates *chairs, int *colors, int size, char *label, char border);
int draw_philosophers_table(struct coordinates center, struct table *table, struct coordinates *cords, struct coordinates *forks);
void draw_box(struct coordinates coordinates);
int draw_chair(struct coordinates cords, int color, char *label, char border, int i);
#endif //PROJEKT_DRAWER_
