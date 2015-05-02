#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<pthread.h>
#include<errno.h>
#include "kernel.h"

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
  int i=0;
  root = (node*) calloc(1, sizeof(node));
  walker = root;
  while( i++ < LARGE) {
    walker->next = (node*) calloc(1, sizeof(node));
    walker->something[i%SMALL] = i;
    walker = walker->next;
  }
  walker->next = NULL;
  walker = root; 
  while(walker->next) {
    swap = walker->next;
    free(walker);
    walker = swap;
  }
  free(walker);
}

void kernelMem(void* ptr) {
  int i;
  for(i=0;i<4;i++)
    memoryAssault();
}
