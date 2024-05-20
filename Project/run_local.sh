

g++ -O2 -o serial serial.cpp
g++ -O2 -o standard standard.cpp
g++ -O2 -o standard_omp standard_omp.cpp -fopenmp


export OMP_NUM_THREADS=4
OMP_PLACES=cores

./serial
./standard
./standard_omp

rm serial
rm standard
rm standard_omp