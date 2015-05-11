#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<mpi.h>
#include "kernel.h"

#define NUM_ROUNDS 10*1000
#define BUFFSZ 32*1024
#define BUFFLEN BUFFSZ/sizeof(double)

void col(thdata* comm) {
	double* sbuff;
	double* rbuff;
	int i;
	sbuff = calloc(BUFFLEN,sizeof(double));
	rbuff = calloc(BUFFLEN,sizeof(double));
	/* give all but rank zero a little extra work 
         * may not be enough to matter - need to tune this
         */
        if( comm->rank ) {
		for(i=0; i<BUFFLEN; i++)
			sbuff[i] = i*sqrt(42)+42;
	}
	for(i=0; i<NUM_ROUNDS; i++) {
		MPI_Allreduce(sbuff, rbuff, BUFFLEN, MPI_DOUBLE, MPI_MAX,
				MPI_COMM_WORLD);
	}
	free(sbuff);
	free(rbuff);
}

void kernel_entry(void* ptr) {
	thdata *comm = (thdata *) ptr;
	col(comm);
	return;
}
