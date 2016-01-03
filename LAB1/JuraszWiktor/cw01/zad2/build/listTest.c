#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>
#include <unistd.h> // times

void timeChecker(clock_t startTime, clock_t *currReal, struct tms *currTimes){
	clock_t prevReal = *currReal;
	clock_t prevUser = currTimes->tms_utime;
	clock_t prevSystem = currTimes->tms_stime;
	*currReal=times(currTimes);
	printf("\n----------------------------TIMES----------------------------");
	printf("\nREAL TIME:\t%ld \t\t%ld \n", (*currReal - startTime) , (*currReal - prevReal) );
	printf("USER TIME:\t%ld \t\t%ld \n",(intmax_t) currTimes->tms_utime , (currTimes->tms_utime - prevUser) );
	printf("SYSTEM TIME:\t%ld \t%ld \n\n", currTimes->tms_stime , (currTimes->tms_stime - prevSystem) );
}

char* concatenate(char *p, char *q) {
	int c, d;
	c = 0;
	while (p[c] != '\0') {
		c++;  	
	}  
	d = 0;
	while (q[d] != '\0') {
		p[c] = q[d];
		d++;
		c++;	
	}
	p[c] = '\0';
	return p;
}


void randomString(char *dest, size_t length) {
	char charset[] = "0123456789"
        	        "abcdefghijklmnopqrstuvwxyz"
                	"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	while (length-- > 0) {
		size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
		*dest++ = charset[index];
	}
	*dest = '\0';
}


struct Node* randomNode(){
	struct Node* node = malloc(sizeof(struct Node));
	node->surname = (char*)malloc(sizeof(char));
	randomString(node->surname,sizeof(node->surname));

	char *email = (char*)malloc(sizeof(char));
	char *atcom="@mail.com";		
	randomString(email,sizeof(email));
	node->email = concatenate(email,atcom);

	struct Address a2;
        a2.town="Krakow";
        a2.street="Dluga";
        a2.home_number=6;
        a2.post_code="12-123";
        node->address=a2;
        node->name="Jan";
        node->birth_date="11.11.1992";
        node->phone="1234567892";
	return node;
}

int main(){
	struct tms *currTimes = (struct tms*)malloc(sizeof(struct tms));
	clock_t startTime = times(currTimes);
	clock_t currReal = startTime;
	printf("%jd",startTime);
	timeChecker(startTime, &currReal, currTimes);  
	int i;		
//	for(i= 1; i > 0; i++);
	printf("%jd",currTimes->tms_stime);
	struct List *list = createList();
	//struct Address a1 = {*town = "Krakow", *street = "Dluga", home_number = 6, *post_code = "11-111"};
	//struct Addres a2 = {"Warszawa", "Marszalkowska", 10, "12-345"};
	struct Address a1;
	struct Node *n1 = malloc(sizeof(struct Node));
	a1.town="Krakow";
	a1.street="Dluga";
	a1.home_number=6;
	a1.post_code="12-123";
	n1->address=a1;
	n1->name="Jan";
	n1->surname="Kowlaski";
	n1->birth_date="11.11.1999";
	n1->email="mail@email.com";
	n1->phone="123456789";
     	struct Address a2;
        struct Node *n2 = malloc(sizeof(struct Node));
        a2.town="Krakow2";
        a2.street="Dluga2";
        a2.home_number=62;
        a2.post_code="12-1232";
        n2->address=a2;
        n2->name="Jan2";
        n2->surname="Nowak";
        n2->birth_date="11.11.19992";
        n2->email="mail2@email.com";
        n2->phone="1234567892";
	struct Address a3;
        struct Node *n3 = malloc(sizeof(struct Node));
        a3.town="Miasto3";
        a3.street="ulica3";
        a3.home_number=623;
        a3.post_code="33-333";
        n3->address=a3;
        n3->name="Imie3";
        n3->surname="Zamoyski";
        n3->birth_date="11.11.1993";
        n3->email="mail3@email.com";
        n3->phone="333333333";
	struct Address a4;
        struct Node *n4 = malloc(sizeof(struct Node));
        a4.town="Miasto4";
        a4.street="ulica4";
        a4.home_number=6234;
        a4.post_code="44-444";
        n4->address=a4;
        n4->name="Imie4";
        n4->surname="Biel";
        n4->birth_date="11.11.1994";
        n4->email="mail4@email.com";
        n4->phone="444444444";
	struct Address a5;
        struct Node *n5 = malloc(sizeof(struct Node));
        a5.town="Miasto5";
        a5.street="ulica5";
        a5.home_number=62345;
        a5.post_code="55-555";
        n5->address=a5;
        n5->name="Imie5";
        n5->surname="Coooooooooo";
        n5->birth_date="11.11.1995";
        n5->email="mail5@email.com";
        n5->phone="555555555";
	addContact(list,n1);
	addContact(list,n2);
	addContact(list,n3);
	addContact(list,n4);
	timeChecker(startTime, &currReal, currTimes);  
	printf("\nHEAD:");
	printNode(list->head);
	printf("\n////////////////////////");
	printf("\nTAIL:");
	printNode(list->tail);
	printf("\n////////////////////////");
	printf("\nWHOLE  LIST:");
	printList(list);
	printf("\n////////////////////////");
	sortBySurname(list);
	printf("\nAFTER SORTING");
	printList(list);
	printf("\n////////////////////////");
	printf("\nHEAD:");
	printNode(list->head);
	printf("\n////////////////////////");
	printf("\nTAIL: ");
	printNode(list->tail);
	printf("\n////////////////////////");
//	printf("\n-------------------------------\n");
	char *e = "mail2@email.com";
	deleteContactByEmail(&list,e);
	printf("\nAFTER DELETING MAIL2@EMAIL.COM");
	printList(list);
 	printf("\n////////////////////////");
	addContact(list,n5);
	printf("\nAFTER Cooooooooo ADDED");
	printList(list);
	printf("\n////////////////////////");
	printf("\nHEAD:");
	printNode(list->head);
	printf("\n////////////////////////");
	printf("\nTAIL: ");
	printNode(list->tail);
	printf("\n////////////////////////");
	printf("\nWHOLE LIST: ");
	printList(list);
	printf("\n////////////////////////");
	timeChecker(startTime, &currReal, currTimes);  
	printf("ADDING 5000 NODES");
	
	for(i=0;i<100000;i++)addContact(list,randomNode());
	printf("\n///////////////////////");
	printf("\nSORTED AGAIN");
	sortBySurname(list);
//	printList(list);
	printf("\n////////////////////////");
	printf("\nHEAD:");
	printNode(list->head);
	printf("\n////////////////////////");
	printf("\nTAIL: ");
	printNode(list->tail);
	printf("\n////////////////////////");
	printf("\n SEARCHING FOR MAIL3");
	printNode(searchByEmail(list,"mail3@email.com"));	
	printf("\n////////////////////////");
	printf("\nSEARCH FOR MAIL2(IT WAS REMOVED)");
	printNode(searchByEmail(list,"mail2@email.com"));	
	printf("\n////////////////////////");	
	printf("\nTRYING TO DELETE MAIL2 AGAIN");
	deleteContactByEmail(&list,e);
	printf("\n////////////////////////");	
	printf("DELETING WHOLE LIST");
	deleteList(&list);
	printList(list);
	printf("\nHEAD:");
	printNode(list->head);
	printf("\n////////////////////////");
	printf("\nTAIL: ");
	printNode(list->tail);
	printf("\n////////////////////////");
	printf("\n SEARCHING FOR MAIL3");
	printNode(searchByEmail(list,"mail3@email.com"));	
	printf("\n////////////////////////");
	printf("\nSEARCH FOR MAIL2(IT WAS REMOVED)");
	printNode(searchByEmail(list,"mail2@email.com"));	
	printf("\n////////////////////////");
	timeChecker(startTime, &currReal, currTimes); 
        printf("%ld",times(currTimes));	
	
	return 0;
}
