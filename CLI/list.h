#ifndef _LIST_H_
#define _LIST_H_

void addNode(char* data, int size);
struct Node* searchNode(int n);
void deleteList(void);

struct Node{
	int size;
	char* data;
	struct Node* next;
};

#endif 