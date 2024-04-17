#!/bin/bash -l
# The -l above is required to get the full environment with modules

#SBATCH -J run
#SBATCH -t 0:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=32
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH -e error_file.e

# Run the executable file 
#echo Running serial_sum...
#cc -o serial_sum serial_sum.c -openmp #Do not use any optimization here, messes with the timing.
#srun -n 1 ./serial_sum > serial_sum_output
#rm serial_sum

cc -o omp_sum omp_sum.c -openmp #Do not use any optimization here, messes with the timing.
srun -n 32 ./omp_sum > omp_sum_output.txt
rm omp_sum