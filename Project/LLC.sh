g++ -O2 -o standard standard.cpp
g++ -O2 -o serial serial.cpp

num_particles=2000

sudo perf stat -o profiling_outputs/serial$num_particles.txt -e instructions,cycles,L1-dcache-load-misses,L1-dcache-loads,LLC-loads,LLC-load-misses,LLC-stores,LLC-store-misses -r 10 ./serial

sudo perf stat -o profiling_outputs/standard$num_particles.txt -e instructions,cycles,L1-dcache-load-misses,L1-dcache-loads,LLC-loads,LLC-load-misses,LLC-stores,LLC-store-misses -r 10 ./standard 