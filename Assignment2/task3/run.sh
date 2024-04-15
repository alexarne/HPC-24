#!/bin/bash 
clang -o serial_sum serial_sum.c -fopenmp #Do not use any optimization here, messes with the timing.
./serial_sum