#ifndef LIST_H
#define LIST_H
struct Address{
        char *town;
        char *street;
        int home_number;
        char *post_code;
};

struct Node{
        char *name;
        char *surname;
        char *birth_date;
        char *email;
        char *phone;
        struct Address address;
        struct Node *next;
        struct Node *prev;
};

struct List{
        struct Node *head;
        struct Node *tail;
};
#endif
