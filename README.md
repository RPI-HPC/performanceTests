## performanceTests
Memory intensive and FLOP intensive tests

## contents
- build.sh - build script
- env*.sh - environment setup scripts
- getTime*.sh - run script
- kernelFlop.c - flop intensive kernel
- kernelMem.c - memory intensive kernel
- kernel.h - kernel header
- mpiWork.c - mpi driver
- README.md - this file

## CCI Drp

### build

    source envCCIDrp.sh
    ./build.sh

### run

    sbatch -N 1 -t 5 ./getTimeCCIDrp.sh

### results - power optimized bios settings

    mem workers 2 average 3.60136
    flop workers 2 average 3.91518
