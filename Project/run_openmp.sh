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

for n in 1 2 4 8 16 32 64 128;
do
    export OMP_NUM_THREADS=$n
    OMP_PLACES=cores
    perf stat -o profiling_outputs/standard_omp3000_$n.txt -e instructions,cycles,L1-dcache-load-misses,L1-dcache-loads,LLC-loads,LLC-load-misses,LLC-stores,LLC-store-misses -r 10 ./standard_omp
done