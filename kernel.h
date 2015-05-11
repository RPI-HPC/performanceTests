#ifndef WORK_KERNEL_H_
#define WORK_KERNEL_H_
typedef struct str_thdata {
	int rank;
	int commsz;
	int id;
	int peers;
} thdata;
void kernelMem(void*);
void kernelFlop(void*);
void kernelP2P(void*);
#endif
