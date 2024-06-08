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


##  This sbatch bash script compiles and runs all the different implementations. 
##  MPI: 4 processes are spawned, 16 thread on each.
##  OpenMP: 16 threads are used


CC -O2 -o optimized_omp optimized_omp.cpp -openmp
CC -O2 -o optimized_mpi optimized_mpi.cpp -openmp
CC -O2 -o serial serial.cpp
CC -O2 -o optimized optimized.cpp
mkdir -p output

srun -n 1 ./serial > ./output/output_serial.txt
srun -n 1 ./optimized > ./output/output_optimized.txt

export OMP_NUM_THREADS=16
OMP_PLACES=cores
srun -n 1 ./optimized_omp > ./output/output_omp.txt
srun -n 4 ./optimized_mpi

#Clean-up:
if [ ! -s "error_file.e" ]; then
    rm "error_file.e"
fi
rm optimized_omp
rm optimized
rm optimized_mpi
rm serial
rm slurm-*.out
