#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <dlfcn.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>

#include "kernel.h"

struct thread_args {
	thdata *data;
	void (*entry)(thdata *);
	unsigned long repeat;
};

static unsigned int strtoui_max(const char *str, char **endptr, int base)
{
	unsigned long tmp = strtoul(optarg, endptr, base);

	if (tmp > UINT_MAX)
		return UINT_MAX;
	else
		return (unsigned int) tmp;
}

static void *thread(void *arg)
{
	int r;
	struct thread_args *args = (struct thread_args *) arg;

	for (r=0; r<args->repeat; r++)
		(*args->entry)(args->data);

	return NULL;
}

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
	unsigned int seed = time(NULL);
	pthread_t *threads;
	unsigned int t;
	unsigned int thread_count = 0;
	struct thread_args args;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSz);

	thdata data;
	data.rank = rank;
	data.commsz = worldSz;
	data.id = rank;
	data.peers = 1;

	while ((c = getopt (argc, argv, "k:r:s:t:")) != -1)
		switch (c) {
			case 'k':
				kernel = strdup(optarg);
				break;
			case 'r':
				repeat = strtoul(optarg, NULL, 10);
				break;
			case 's':
				seed = strtoui_max(optarg, NULL, 10);
				break;
			case 't':
				thread_count = strtoui_max(optarg, NULL, 10);
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

	printf("Seed: %d\n", seed);
	srand(seed);

	double t0 = MPI_Wtime();

	if (thread_count == 0) {
		for (r=0; r<repeat; r++)
			(*entry)(&data);
	} else {
		printf("Starting %u threads\n", thread_count);
		threads = calloc(thread_count, sizeof(pthread_t));

		args.repeat = repeat;
		args.data = &data;
		args.entry = entry;

		for (t = 0; t < thread_count; t++) {
			pthread_create(&threads[t], NULL, thread, &args);
		}

		for (t = 0; t < thread_count; t++) {
			pthread_join(threads[t], NULL);
		}

		free(threads);
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
