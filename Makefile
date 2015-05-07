flags="-Wall -g -O3 -fno-omit-frame-pointer "
src="kernelMem.c kernelFlop.c"

CC:=cc
MPICC:=mpicc

all: mpiWork kernels

mpiWork: mpiWork.c
	$(MPICC) $(CFLAGS) $^ -ldl -o $@

kernels/%.so: kernels/%.c
	$(CC) -shared -fpic $(CFLAGS) -I. $^ -o $@

kernels: $(foreach kernel,$(KERNELS),kernels/$(kernel).so)

.PHONY: clean
clean:
	rm -f mpiWork
	rm -f kernels/*.so
