#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:01:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=2
#SBATCH --nodes=1
#SBATCH -e error_file.e

export OMP_NUM_THREADS=4
OMP_PLACES=cores
cc -O2 -o ex1 ex1.c -openmp

srun -n 1 ./ex1 > output.txt
