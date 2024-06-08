#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:20:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p main
#SBATCH --ntasks-per-node=64
#SBATCH --cpus-per-task=1
#SBATCH --nodes=2
#SBATCH -e error_file.e
cd ..

## CRAYPAT
module load perftools-base
module load perftools-lite

export OMP_NUM_THREADS=1
OMP_PLACES=cores

CC -O2 -o optimized_mpi.x optimized_mpi.cpp -openmp

pat_build optimized_mpi.x+pat
mkdir -p craypat_outputs
for i in {1..2};
do
    for p in 1 2 4 8 16 32 64 128; do
    srun -n $p ./optimized_mpi.x > craypat_outputs/craypat_single${p}_iter$i.txt
    done;
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