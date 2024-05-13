#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH --nodes=2
#SBATCH -e error_file16.e

cc -O2 -o fox16 fox.c
srun -n 16 ./fox16 > output_16.txt

#Clean-up:
if [ ! -s "error_file16.e" ]; then
    rm "error_file16.e"
fi
rm fox16
rm slurm-*.out
