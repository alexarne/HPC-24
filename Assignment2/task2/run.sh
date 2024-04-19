#!/bin/bash -l

#SBATCH -J run
#SBATCH -t 00:05:00
#SBATCH -A edu24.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=128
#SBATCH --nodes=1
#SBATCH -e error_file.e
#SBATCH --output=output.log

for n in 1 32 64 128;
do
    export OMP_NUM_THREADS=$n
    OMP_PLACES=cores
    cc -O2 -o stream stream.c -openmp
    for i in $(seq 1 5);
    do
        srun -n 1 ./stream > ./results/results_${n}_${i}.txt
    done
done

export OMP_NUM_THREADS=128
OMP_PLACES=cores
cc -O2 -o stream stream.c -openmp
cc -O2 -o stream_static stream_static.c -openmp
cc -O2 -o stream_dynamic stream_dynamic.c -openmp
for i in $(seq 1 5);
do
    srun -n 1 ./stream > ./results_guided128/result_${i}.txt
    srun -n 1 ./stream_static > ./results_static128/result_${i}.txt
    srun -n 1 ./stream_dynamic > ./results_dynamic128/result_${i}.txt
done