#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p main
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=16
#SBATCH --nodes=1
#SBATCH -e error_file.e

## CRAYPAT
module load perftools-base
module load perftools-lite

n=16

export OMP_NUM_THREADS=$n
OMP_PLACES=cores

CC -O2 -o standard_mpi.x standard_mpi.cpp -openmp

pat_build standard_mpi.x+pat

srun ./standard_mpi.x > craypatoutputs/craypat_$n.txt

pat_report

#Clean-up:
if [ ! -s "error_file.e" ]; then
    rm "error_file.e"
fi
rm -rf standard_mpi.x+*
rm standard_mpi.x
rm slurm_*