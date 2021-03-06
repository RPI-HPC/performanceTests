#include "kernel.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void doLotsOfWork() {
	unsigned int seed;
	int dim = 1000;
	int i,j;
	double** A;
	double* b;
	double* c;

	A = calloc(dim, sizeof(double*));
	for(i=0;i<dim;i++)
		A[i] = calloc(dim, sizeof(double));
	b = calloc(dim, sizeof(double));
	c = calloc(dim, sizeof(double));

	for(i=0;i<dim;i++)
		for(j=0;j<dim;j++)
			A[i][j] = rand_r(&seed);
	for(i=0;i<dim;i++)
		b[i] = rand_r(&seed);

	for(i=0;i<dim;i++)
		for(j=0;j<dim;j++)
			c[i] += A[i][j]*b[j];

	for(i=0;i<dim;i++)
		free(A[i]);

	free(A);
	free(b);
	free(c);
}

void kernel_entry(void* ptr) {
	doLotsOfWork();
}
