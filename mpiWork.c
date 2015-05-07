#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "kernel.h"

int main(int argc, char** argv) {
	int rank;
	int worldSz;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSz);
	thdata* data = (thdata*) calloc(1,sizeof(thdata));
	data->rank = rank;
	data->commsz = worldSz;
	data->id = rank;
	data->peers = 1;
	double t0 = MPI_Wtime();
#ifdef MEM
	kernelMem((void*) data);
#elif FLOP
	kernelFlop((void*) data);
#else
	#error "Unknown configuration - compile with either -DMEM or -DFLOP"
#endif
	MPI_Barrier(MPI_COMM_WORLD);
	if( !rank )
		fprintf(stderr, "realTime %f\n", MPI_Wtime()-t0);
	free(data);
	MPI_Finalize();
	return 0;
}
