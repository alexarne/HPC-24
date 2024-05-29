#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p main
#SBATCH --ntasks-per-node=128
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH -e error_file.e


module load perftools-base
module load perftools-lite

CC -O2 -o standard_mpi.x standard_mpi.cpp

pat_build standard_mpi.x+pat

srun ./standard_mpi.x

pat_report