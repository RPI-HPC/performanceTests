#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "kernel.h"

int main(int argc, char** argv) {
	int c;
	unsigned long repeat = 1;
	unsigned long r;
	int rank;
	int worldSz;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSz);

	thdata* data = calloc(1,sizeof(thdata));
	data->rank = rank;
	data->commsz = worldSz;
	data->id = rank;
	data->peers = 1;

	while ((c = getopt (argc, argv, "r:")) != -1)
		switch (c) {
			case 'r':
				repeat = strtoul(optarg, NULL, 10);
		}

	double t0 = MPI_Wtime();

	for (r=0; r<repeat; r++) {
#ifdef MEM
		kernelMem((void*) data);
#elif FLOP
		kernelFlop((void*) data);
#else
	#error "Unknown configuration - compile with either -DMEM or -DFLOP"
#endif
	}

	MPI_Barrier(MPI_COMM_WORLD);
	if( !rank )
		fprintf(stderr, "realTime %f\n", MPI_Wtime()-t0);
	free(data);
	MPI_Finalize();
	return 0;
}
