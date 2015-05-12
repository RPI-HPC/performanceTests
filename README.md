## performanceTests
Memory intensive and FLOP intensive tests

## Select contents
- env*.sh - environment setup scripts
- getTime*.sh - run script
- kernel.h - kernel header
- mpiWork.c - mpi driver
- kernels/ - test kernels (see below)

## Test kernels
- flop1 - flop intensive kernel
- mem1 - memory intensive kernel
- p2pcomm1 - peer to peer communication kernel
- colcomm1 - collective communication kernel

## CCI Drp

### build

    source envCCIDrp.sh
    make KERNELS="flop1 mem1"

### run

    sbatch -N 1 -t 5 ./getTimeCCIDrp.sh
