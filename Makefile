CC:=cc
MPICC:=mpicc

CFLAGS:=-Wall -g -O3 -fno-omit-frame-pointer

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
