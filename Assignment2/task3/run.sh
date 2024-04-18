#!/bin/bash -l
# The -l above is required to get the full environment with modules

#SBATCH -J run
#SBATCH -t 0:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=64
#SBATCH --nodes=1
#SBATCH -e error_file.e

# Run the executable file 
#echo Running serial_sum...
#cc -o serial_sum serial_sum.c -openmp #Do not use any optimization here, messes with the timing.
#srun -n 1 ./serial_sum > serial_sum_output
#rm serial_sum

export OMP_NUM_THREADS=32
OMP_PLACES=cores

cc sum.c -openmp #Do not use any optimization here, messes with the timing.
srun -n 1 ./a.out > sum_output.txt
rm a.out
