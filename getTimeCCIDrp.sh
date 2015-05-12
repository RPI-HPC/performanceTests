#!/bin/bash -e
run() {
  srun --mpi=none -n $2 $1 &>> work${2}.log
}
mem() {
  run "./mpiWork -k kernels/mem1.so -r 4" $1
}
flop() {
  run "./mpiWork -k kernels/flop1.so -r 400" $1
}
p2p() {
  export MV2_ENABLE_AFFINITY=0
  run "./mpiWork -k kernels/p2pcomm1.so -r 30 -t 2" $1
  unset MV2_ENABLE_AFFINITY
}
col() {
  run "./mpiWork -k kernels/colcomm1.so -r 10" $1
}
getAvg() {
  awk '/realTime/ {sum+=$2; cnt+=1;} END {print "average " sum/cnt}' work${1}.log
}

declare -a fns
fns[1]="mem"
fns[2]="flop"
fns[3]="p2p"
fns[4]="col"

workers=2
for i in "${!fns[@]}"; do
  cat /dev/null > work${workers}.log
  echo -n "${fns[i]} workers $workers "
  for j in {0..4}; do
    ${fns[$i]} $workers
  done
  getAvg $workers
done
