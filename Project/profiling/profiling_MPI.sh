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
cd ..

## CRAYPAT
module load perftools-base
module load perftools-lite

export OMP_NUM_THREADS=16
OMP_PLACES=cores

CC -O2 -o optimized_mpi.x optimized_mpi.cpp -openmp

pat_build optimized_mpi.x+pat

mkdir -p craypat_outputs
for i in {1..2};
do
    srun -n 1 ./optimized_mpi.x > craypat_outputs/craypat_1_iter$i.txt
    srun -n 2 ./optimized_mpi.x > craypat_outputs/craypat_2_iter$i.txt
    srun -n 4 ./optimized_mpi.x > craypat_outputs/craypat_4_iter$i.txt
    srun -n 8 ./optimized_mpi.x > craypat_outputs/craypat_8_iter$i.txt
    srun -n 12 ./optimized_mpi.x > craypat_outputs/craypat_12_iter$i.txt
    srun -n 16 ./optimized_mpi.x > craypat_outputs/craypat_16_iter$i.txt
done

pat_report

#Clean-up:
rm -rf optimized_mpi.x+*
rm optimized_mpi.x
rm optimized_mpi.x+orig
cd profiling
if [ ! -s "error_file.e" ]; then
    rm "error_file.e"
fi
rm slurm-*