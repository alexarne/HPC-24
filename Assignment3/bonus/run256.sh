#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=128
#SBATCH --cpus-per-task=1
#SBATCH --nodes=2
#SBATCH -e error_file256.e

cc -O2 -o fox256 fox.c
srun -n 256 ./fox256 > output_256.txt

#Clean-up:
if [ ! -s "error_file256.e" ]; then
    rm "error_file256.e"
fi
rm fox256
rm slurm-*.out
