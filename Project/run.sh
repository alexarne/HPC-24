#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p main
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=64
#SBATCH --nodes=1
#SBATCH -e error_file.e
#SBATCH --output=output.log


CC -O2 -o standard_omp standard_omp.cpp -openmp

for n in 1 4 16 64;
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
rm slurm-*.out
rm output.log