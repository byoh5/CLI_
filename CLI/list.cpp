//#include "list.h"
//#include <iostream>
#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"

struct Node{
	int size;
	char* data;
	struct Node* next;
};


void addNode(char* data, int size);
struct Node* searchNode(int n);
void deleteList(void);

struct Node* head;

// apending
void addNode(char* data, int size){

	if (head == NULL){
		head = (struct Node*)malloc(sizeof(struct Node));
		head->size = size;
		head->data = new char[size];
		memcpy(head->data, data, size);
		head->next = NULL;
	}
	else{
		struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
		newNode->size = size;
		newNode->data = new char[size];
		memcpy(newNode->data, data, size);
		newNode->next = NULL;

		struct Node *cur = head;
		while (cur){
			if (cur->next == NULL){
				cur->next = newNode;
				return;
			}
			cur = cur->next;
		}
	}
}

struct Node* searchNode(int n){
	struct Node *cur = head;
	int i;
	for (i = 0; i < n; i++)	cur = cur->next;
	return cur;
}

void deleteList(void){
	struct Node *temp;
	struct Node *cur = head;
	while (head != NULL){
		temp = head;
		head = head->next;
		if (temp->data) delete temp->data;
		if (temp) delete temp;
	}
}