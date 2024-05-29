#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=64
#SBATCH --nodes=1
#SBATCH -e error_file.e


CC -O2 -o standard_omp standard_omp.cpp -openmp
CC -O2 -o serial serial.cpp
CC -O2 -o standard standard.cpp

srun -n 1 ./serial > ./output/output_serial.txt
srun -n 1 ./standard > ./output/output_standard.txt
for n in 1 4 16 64;
do
    export OMP_NUM_THREADS=$n
    OMP_PLACES=cores
    srun -n 1 ./standard_omp > ./output/output_omp_$n.txt
done

#Clean-up:
if [ ! -s "error_file.e" ]; then
    rm "error_file.e"
fi
rm standard_omp
rm standard
rm serial
rm slurm-*.out
