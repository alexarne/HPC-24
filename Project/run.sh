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


CC -O2 -o standard_omp standard_omp.cpp -openmp
CC -O2 -o standard_mpi standard_mpi.cpp -openmp
CC -O2 -o serial serial.cpp
CC -O2 -o standard standard.cpp
mkdir -p output

srun -n 1 ./serial > ./output/output_serial.txt
srun -n 1 ./standard > ./output/output_standard.txt

export OMP_NUM_THREADS=16
OMP_PLACES=cores
srun -n 1 ./standard_omp > ./output/output_omp.txt
srun -n 4 ./standard_mpi > ./output/output_mpi.txt

#Clean-up:
if [ ! -s "error_file.e" ]; then
    rm "error_file.e"
fi
rm standard_omp
rm standard
rm standard_mpi
rm serial
rm slurm-*.out
