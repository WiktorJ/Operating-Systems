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

struct List* createList();
void deleteList(struct List **list);
void addContact(struct List *list, struct Node *newNode);
void deleteContactByEmail(struct List **list, char *email);
struct Node* searchByEmail(struct List *list, char *email);
void mergeSort(struct Node **headRef);
struct Node* merge(struct Node *a, struct Node *b);
void frontBackSplit(struct Node *source, struct Node **frontRef, struct Node **backRef);
void sortBySurname(struct List *list);
void printNode(struct Node *tmp);
void printList(struct List *list);
#endif
