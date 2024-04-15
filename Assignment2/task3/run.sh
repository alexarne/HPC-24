#!/bin/bash 
echo Running serial_sum...
clang -o serial_sum serial_sum.c -fopenmp #Do not use any optimization here, messes with the timing.
./serial_sum
rm serial_sum

echo Running omp_sum...
clang -o omp_sum omp_sum.c -fopenmp #Do not use any optimization here, messes with the timing.
./omp_sum
rm omp_sum