#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:20:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p main
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=128
#SBATCH --nodes=1
#SBATCH -e error_file.e

CC -O2 -o standard_omp standard_omp.cpp -openmp

## CRAYPAT
module load perftools-base
module load perftools-lite

CC -O2 -o standard_omp.x standard_omp.cpp -openmp

pat_build standard_omp.x+pat

for i in {1..10};
do
    for p in 1 2 4 8 16 32 64 128; do
    export OMP_NUM_THREADS=$p
    OMP_PLACES=cores
    srun -n 1 ./standard_omp.x > craypat_outputs/craypat_omp${p}_iter$i.txt
    done;
done