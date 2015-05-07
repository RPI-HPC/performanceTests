#include "kernel.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void doLotsOfWork() {
	int dim = 1000;
	int i,j;
	double** A;
	double* b;
	double* c;

	A = (double**) calloc(dim, sizeof(double*));
	for(i=0;i<dim;i++)
		A[i] = (double*) calloc(dim, sizeof(double));
	b = (double*) calloc(dim, sizeof(double));
	c = (double*) calloc(dim, sizeof(double));

	srand(1);

	for(i=0;i<dim;i++)
		for(j=0;j<dim;j++)
			A[i][j] = rand();
	for(i=0;i<dim;i++)
		b[i] = rand();

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
