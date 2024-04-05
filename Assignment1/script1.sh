srun -n 1 perf stat -e instructions ./matrix\_multiply.out
srun -n 1 perf stat -e cycles ./matrix\_multiply.out
srun -n 1 perf stat -e L1-dcache-load-misses ./matrix\_multiply.out
