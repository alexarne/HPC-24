#!/bin/bash 
for n in 1 32 64 128;
do  
    echo Running case: $n threads.
    export OMP_NUM_THREADS=$n
    cc -O2 -o stream stream.c -fopenmp
    #clang -O2 -o stream stream.c -fopenmp
    for i in $(seq 1 5);
    do
        ./stream > results.txt
        python3 record.py
    done
done
python3 plot_results.py
rm avg.csv
rm results.txt
rm ./stream
