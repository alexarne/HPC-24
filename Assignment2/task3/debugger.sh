#!/bin/bash -l
# The -l above is required to get the full environment with modules

#SBATCH -J run
#SBATCH -t 0:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH -e error_file.e

cc debugger.c -openmp
srun -n 1 ./a.out > output.out

