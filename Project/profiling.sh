#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:20:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p main
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=16
#SBATCH --nodes=4
#SBATCH -e error_file.e

## CRAYPAT
module load perftools-base
module load perftools-lite

export OMP_NUM_THREADS=16
OMP_PLACES=cores

CC -O2 -o standard_mpi.x standard_mpi.cpp -openmp

pat_build standard_mpi.x+pat

for i in {1..10};
do
    srun -n 1 ./standard_mpi.x > craypat_outputs/craypat_1_iter$i.txt
    srun -n 2 ./standard_mpi.x > craypat_outputs/craypat_2_iter$i.txt
    srun -n 4 ./standard_mpi.x > craypat_outputs/craypat_4_iter$i.txt
    srun -n 8 ./standard_mpi.x > craypat_outputs/craypat_8_iter$i.txt
    srun -n 12 ./standard_mpi.x > craypat_outputs/craypat_12_iter$i.txt
    srun -n 16 ./standard_mpi.x > craypat_outputs/craypat_16_iter$i.txt
done

pat_report

#Clean-up:
if [ ! -s "error_file.e" ]; then
    rm "error_file.e"
fi
rm -rf standard_mpi.x+*
rm standard_mpi.x
rm slurm-*