#!/bin/bash -e
flags="-Wall -g -O3 -fno-omit-frame-pointer "
src="kernelMem.c kernelFlop.c"
set -x
mpicc $flags -DMEM mpiWork.c $src -o mpiWorkMem
mpicc $flags -DFLOP mpiWork.c $src -o mpiWorkFlop
