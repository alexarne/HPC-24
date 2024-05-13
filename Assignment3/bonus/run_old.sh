#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
<<<<<<< HEAD
#SBATCH -p shared
#SBATCH --ntasks-per-node=2
=======
#SBATCH -p main
#SBATCH --ntasks-per-node=64
>>>>>>> f81506f97a00ac9fb10c91492b858a619e734968
#SBATCH --cpus-per-task=1
#SBATCH --nodes=2
#SBATCH -e error_file.e

cc -O2 -o fox fox_old.c
<<<<<<< HEAD
srun -n 4 ./fox > output.txt
=======
for n in 16 36 64;
do
    srun -n $n ./fox > output_$n.txt
done
>>>>>>> f81506f97a00ac9fb10c91492b858a619e734968

#Clean-up:
if [ ! -s "error_file.e" ]; then
    rm "error_file.e"
fi
rm fox
rm slurm-*.out
