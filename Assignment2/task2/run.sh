#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:10:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=64
#SBATCH --nodes=1
#SBATCH -e error_file.e

for n in 1 32 64 128;
do
    echo Running case: $n threads.
    export OMP_NUM_THREADS=$n
    OMP_PLACES=cores
    cc -O2 -o stream stream.c -openmp
    for i in $(seq 1 5);
    do
        srun -n 1 ./stream > ./results/results_${n}_${i}.txt
    done
done

