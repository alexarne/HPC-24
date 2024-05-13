#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:01:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=64
#SBATCH --cpus-per-task=1
#SBATCH --nodes=2
#SBATCH -e error_file.e

cc -O2 -o fox fox_dyn.c

for n in 16;
do
    srun -n $n ./fox > output_$n.txt
done

#Clean-up:
if [ ! -s "error_file.e" ]; then
    rm "error_file.e"
fi
rm fox
rm slurm-*.out
