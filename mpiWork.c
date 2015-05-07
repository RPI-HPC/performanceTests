#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <dlfcn.h>

#include "kernel.h"

int main(int argc, char** argv) {
	int rc = EXIT_FAILURE;
	int c;
	unsigned long repeat = 1;
	unsigned long r;
	int rank;
	int worldSz;
	char *kernel = NULL;
	void *handle;
	void (*entry)(thdata *);
	const char *error;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSz);

	thdata data;
	data.rank = rank;
	data.commsz = worldSz;
	data.id = rank;
	data.peers = 1;

	while ((c = getopt (argc, argv, "k:r:")) != -1)
		switch (c) {
			case 'r':
				repeat = strtoul(optarg, NULL, 10);
				break;
			case 'k':
				kernel = strdup(optarg);
		}

	if (kernel == NULL) {
		fprintf(stderr, "Must supply kernel to run!\n");
		goto failure;
	}

	handle = dlopen(kernel, RTLD_NOW);
	if (handle == NULL) {
		fprintf(stderr, "Unable to load kernel: %s\n", dlerror());
		goto failure;
	}

	*(void **) (&entry) = dlsym(handle, "kernel_entry");

	error = dlerror();

	if (error != NULL) {
		fprintf(stderr, "Unable to start kernel: %s\n", error);
		goto failure;
	}

	double t0 = MPI_Wtime();

	for (r=0; r<repeat; r++) {
		(*entry)(&data);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	if( !rank )
		fprintf(stderr, "realTime %f\n", MPI_Wtime()-t0);

	rc = EXIT_SUCCESS;

failure:
	if (kernel != NULL)
		free(kernel);
	MPI_Finalize();

	return rc;
}
