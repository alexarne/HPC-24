#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:01:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --nodes=2
#SBATCH -e error_file.e

cc -o pp2 ping_pong.c
srun -n 2 ./pp2 > output2.txt

rm pp2
if [ ! -s "error_file.e" ]; then
    rm "error_file.e"
fi
rm slurm-*.out
