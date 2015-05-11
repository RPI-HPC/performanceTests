#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<pthread.h>
#include<errno.h>
#include<math.h>
#include "kernel.h"

#define NUM_ROUNDS 10*1000
#define HWTHREADS 1

void doLotsOfWork() {
	int large = 1000*1000;
	int i,j;
	double* arr;
	arr = calloc(large, sizeof(double));
	for(i=0;i<large/4;i++)
		for(j=0;j<large;j++)
			arr[j] = sqrt(i+j*3.21)/123.3+j;
	free(arr);
}

#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

void pin(int id) {
	int s, j;
	int logicalCore;
	cpu_set_t cpuset;
	pthread_t thread = pthread_self();

	CPU_ZERO(&cpuset);

#ifndef THREADS_PER_CORE
	#define THREADS_PER_CORE 1
#endif

#if THREADS_PER_CORE == 4
	logicalCore = id;
#elif THREADS_PER_CORE == 2
	logicalCore = (id*THREADS_PER_CORE)+((id)%THREADS_PER_CORE);
#elif THREADS_PER_CORE == 1
	logicalCore = (id*HWTHREADS)%8;
#else
	#error THREADS_PER_CORE must be 1, 2, or 4
#endif
	CPU_SET(logicalCore, &cpuset);

	s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
	if (s != 0)
		handle_error_en(s, "pthread_setaffinity_np");

	/* Check the actual affinity mask assigned to the thread */

	s = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
	if (s != 0)
		handle_error_en(s, "pthread_getaffinity_np");

	for (j = 0; j < CPU_SETSIZE; j++)
		if (CPU_ISSET(j, &cpuset))
			fprintf(stderr, "worker %d CPU %d\n", id, j);
}

inline int isFirstPeer(thdata* comm) {
	return (comm->id % comm->peers == 0);
}

inline int isLastPeer(thdata* comm) {
	const int thread = comm->id % comm->peers;
	return (thread == comm->peers-1);
}

inline int isFirstProc(thdata* comm) {
	return (comm->rank == 0);
}

inline int isLastProc(thdata* comm) {
	return (comm->rank == comm->commsz-1);
}

void p2p(thdata* comm) {
	MPI_Request req[2];
	MPI_Status stat[2];
	int prev, next;
	int buf;
	int sendTag, recvTag;
	int i;
	/* uncomment this to observe pinning placement */
	/*
	doLotsOfWork(); 
	*/
	/* mpi rank target */
	prev = next = comm->rank;
	if( isFirstPeer(comm) ) prev--;
	if( isFirstProc(comm) && isFirstPeer(comm) ) prev = comm->commsz-1;

	if( isLastPeer(comm) ) 
		next = (comm->rank+1) % comm->commsz;
	/* thread id target */
	recvTag = comm->id-1;
	sendTag = comm->id;
	if( recvTag < 0 ) recvTag = (comm->peers*comm->commsz)-1;
	for(i=0; i<NUM_ROUNDS; i++) {
		MPI_Irecv(&buf, 1, MPI_INT, prev, recvTag, MPI_COMM_WORLD, &req[0]);
		MPI_Isend(&comm->id, 1, MPI_INT, next, sendTag, MPI_COMM_WORLD, &req[1]);
		MPI_Waitall(2, req, stat);
	}
}

void kernel_entry(void* ptr) {
	thdata *comm = (thdata *) ptr;
	pin(comm->id);
	p2p(comm);
	return;
}
