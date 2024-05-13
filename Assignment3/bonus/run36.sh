#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=36
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH -e error_file36.e

cc -O2 -o fox36 fox.c
srun -n 36 ./fox36 > output_36.txt

#Clean-up:
if [ ! -s "error_file36.e" ]; then
    rm "error_file36.e"
fi
rm fox36
rm slurm-*.out
