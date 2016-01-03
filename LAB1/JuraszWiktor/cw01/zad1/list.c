#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#include "list.h"

typedef int bool;
#define true 1
#define false 0

struct List* createList(){	
	struct List *list = malloc(sizeof(struct List));
	//list->head =  malloc(sizeof(struct Node));
	//list->head->next = NULL;
	//list->head->prev= NULL; 
	//list->tail= list->head;
	list->head=NULL;
	list->tail=NULL;
	return list;	
}


void deleteList(struct List **list){
	struct Node* tmp=(*list)->head;;
		while(tmp!=(*list)->tail){
			struct Node *tmp2 = tmp->next;
			tmp->prev=tmp->next=NULL;
			//tmp =(*list)->head->next;
			free(tmp);
			tmp=tmp2;	
		}
	tmp->prev=tmp->next=NULL;
	free(tmp);
	(*list)->tail=(*list)->head=NULL;
	free((*list));
}

void addContact(struct List *list,struct Node *newNode){
	if(list!=NULL&&newNode!=NULL){
			newNode->next=NULL;
			newNode->prev=list->tail;
		if(list->tail!=NULL){
			list->tail->next=newNode;
			list->tail=newNode;
		} else {
			list->tail=newNode;
			list->head=newNode;	
		}
	} else {
		if(list==NULL) printf("%s", "Given LIST is NULL");
		if(newNode==NULL)printf("Given NODE is NULL");
	}
}

void deleteContactByEmail(struct List **list, char *email){
	if(list!=NULL){
		struct Node *tmp = (*list)->head;
		int flag = false;
		while(tmp!=NULL&&!flag){
			if(!strcmp(tmp->email,email)){
				if(tmp->next!=NULL) tmp->next->prev=tmp->prev;
				else (*list)->tail=tmp->prev;
				if(tmp->prev!=NULL) tmp->prev->next=tmp->next;
				else (*list)->head=tmp->next;
				tmp->next=tmp->prev=NULL;
				free(tmp);
				flag=true;				
			}				
			tmp=tmp->next;				
		}
		if(!flag) printf("There is no such an email in LIST");
		else printf("NODE was removed");		
	} else printf("Given LIST is NULL");
}

struct Node* searchByEmail(struct List *list, char *email){
	if(list!=NULL){
		struct Node *tmp = list->head;
		bool flag = false;
		while(tmp!=NULL&&!flag){
			if(!strcmp(tmp->email,email)) return tmp;
			tmp=tmp->next;
		}
		
	} else {
		printf("Give LIST is NULL");
	}
	printf("There is no such a email in LIST");
	return NULL;
}



void mergeSort(struct Node **headRef){
	struct Node *head = *headRef;
	struct Node *a;
	struct Node *b;
	if(head==NULL || head->next==NULL) return;
	frontBackSplit(head,&a,&b);
	mergeSort(&a);
	mergeSort(&b);
	*headRef = merge(a,b);
	head->prev=NULL;
}

struct Node* merge(struct Node *a, struct Node *b){
	struct Node *result;
	if(a==NULL) return b;
	else if(b==NULL) return a;
	
	if(strcmp(a->surname,b->surname)<=0){
		result = a;
		result->next = merge(a->next,b);
		result->next->prev=result;
	} else{
		result = b;
		result->next = merge(a,b->next);
		result->next->prev=result;
	}
	return result;
}

void frontBackSplit(struct Node *source, struct Node **frontRef, struct Node **backRef){
	struct Node *fast;
	struct Node *slow;
	
	if(source==NULL || source->next==NULL){
		*frontRef = source;
		*backRef = NULL;
	} else{
		slow = source;
		fast = source->next;
		while(fast!=NULL){
			fast = fast ->next;
			if(fast!=NULL){
				slow=slow->next;
				fast=fast->next;
			}
		}
	}
	*frontRef = source;
	*backRef = slow->next;
	slow ->next = NULL;
}



void sortBySurname(struct List *list){
	if(list==NULL) return;
	if(list->head==NULL)return;
	if(list->head->next==NULL) return;
	mergeSort(&list->head);
	struct Node *tmp = list->head;
	while(tmp->next!=NULL){
		tmp=tmp->next;
	}
	list->tail=tmp;
}
void printNode(struct Node *tmp){
	if(tmp!=NULL){
		printf("\n");
	    	if(tmp->name!=NULL) printf("%s\n", tmp->name);
           	if(tmp->surname!=NULL) printf("%s\n", tmp->surname);
            	if(tmp->birth_date!=NULL) printf("%s\n", tmp->birth_date);
           	if(tmp->email!=NULL) printf("%s\n", tmp->email);
            	if(tmp->phone!=NULL) printf("%s\n", tmp->phone);
            	struct Address a = tmp->address;
            	if(a.town!=NULL) printf("%s\n", a.town);
            	if(a.street!=NULL) printf("%s\n", a.street);
            	if(a.post_code!=NULL) printf("%s\n", a.post_code);
        	printf("%d\n",a.home_number);
	}
}
void printList(struct List *list){
	if(list!=NULL){
		printf("\n");
		struct Node *tmp;
		tmp = list->head;
		while(tmp!=NULL){
			printNode(tmp);
			tmp=tmp->next;
			printf("---------------------");
		}
		
	} else printf("Given LIST is NULL");
}

