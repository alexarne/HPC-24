#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:02:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p main
#SBATCH --ntasks-per-node=128
#SBATCH --cpus-per-task=1
#SBATCH --nodes=2
#SBATCH -e error_file.e


cc -O2 -o parallel_reduce parallel_reduce.c
cc -O2 -o parallel parallel.c
cc -O2 -o serial serial.c

for n in 8 16 32 64 128 256;
do
    srun -n $n ./parallel_reduce > output/parallel_reduce_${n}.txt
    srun -n $n ./parallel > output/parallel_${n}.txt
done
srun -n 1 ./serial > output/serial.txt


#Clean-up:
if [ ! -s "error_file.e" ]; then
    rm "error_file.e"
fi
rm serial
rm parallel
rm parallel_reduce
rm slurm-*.out