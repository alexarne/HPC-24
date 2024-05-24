
g++ -O2 serial.cpp -pg 

./a.out
gprof a.out gmon.out > analysis_serial.txt
perf stat -e instructions,cycles,L1-dcache-load-misses,L1-dcache-loads,cache-misses,duration_time ./a.out


g++ -O2 standard.cpp -pg 
./a.out
gprof a.out gmon.out > analysis_standard.txt
perf stat -e instructions,cycles,L1-dcache-load-misses,L1-dcache-loads,cache-misses,duration_time ./a.out

rm a.out
rm gmon.out
