#include "kernel.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#define TOTAL 1024*1024*1024
#define SMALL 4*1024
#define LARGE (TOTAL)/(SMALL)

typedef struct Node {
	int something[SMALL];
	struct Node* next;
} node;

void memoryAssault() {
	node* root;
	node* walker;
	node* swap;
	int i = 0;
	root = (node*) calloc(1, sizeof(node));
	walker = root;
	while (i++ < LARGE) {
		walker->next = (node*) calloc(1, sizeof(node));
		walker->something[i%SMALL] = i;
		walker = walker->next;
	}
	walker->next = NULL;
	walker = root;
	while (walker->next) {
		swap = walker->next;
		free(walker);
		walker = swap;
	}
	free(walker);
}

void kernel_entry(void* ptr) {
	memoryAssault();
}
