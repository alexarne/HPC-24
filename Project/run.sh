#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p main
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=128
#SBATCH --nodes=1
#SBATCH -e error_file.e


CC -O2 -o standard_omp standard_omp.cpp -openmp
CC -O2 -o serial serial.cpp

srun -n 1 ./serial > output_serial.txt
for n in 1 4 16 64 128;
do
    export OMP_NUM_THREADS=$n
    OMP_PLACES=cores
    srun -n 1 ./standard_omp > output_$n.txt
done

#Clean-up:
if [ ! -s "error_file.e" ]; then
    rm "error_file.e"
fi
rm standard_omp
rm serial
rm slurm-*.out
