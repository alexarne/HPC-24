#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:01:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH -e error_file.e

cc -o ex1 domainDecom1D.c
srun -n 4 ./ex1 > output.txt

rm ex1
if [ ! -s "error_file.e" ]; then
    rm "error_file.e"
fi
rm slurm-*.out

