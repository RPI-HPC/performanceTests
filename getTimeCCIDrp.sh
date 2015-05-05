#!/bin/bash -e
run() {
  srun --mpi=none -n $2 "$1" &>> work${2}.log
}
mem() { 
  run "./mpiWorkMem" $1
}
flop() { 
  run "./mpiWorkFlop" $1
}
getAvg() {
  awk '/realTime/ {sum+=$2; cnt+=1;} END {print "average " sum/cnt}' work${1}.log 
}

declare -a fns
fns[1]="mem"
fns[2]="flop"

workers=2
for i in "${!fns[@]}"; do   
  cat /dev/null > work${workers}.log
  echo -n "${fns[i]} workers $workers "
  for j in {0..4}; do 
    ${fns[$i]} $workers
  done
  getAvg $workers
done
