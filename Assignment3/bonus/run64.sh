#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=32
#SBATCH --cpus-per-task=1
#SBATCH --nodes=2
#SBATCH -e error_file64.e

cc -O2 -o fox64 fox.c
srun -n 64 ./fox64 > output_64.txt

#Clean-up:
if [ ! -s "error_file64.e" ]; then
    rm "error_file64.e"
fi
rm fox64
rm slurm-*.out
