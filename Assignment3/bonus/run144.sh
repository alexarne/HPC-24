#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=72
#SBATCH --cpus-per-task=1
#SBATCH --nodes=2
#SBATCH -e error_file144.e

cc -O2 -o fox144 fox.c
srun -n 144 ./fox144 > output_144.txt

#Clean-up:
if [ ! -s "error_file144.e" ]; then
    rm "error_file144.e"
fi
rm fox144
rm slurm-*.out
