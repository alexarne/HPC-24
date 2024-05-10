#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define SEED     921

int main(int argc, char* argv[]) {
    int local_count = 0, flip = 1 << 28;
    double x, y, z, pi;
    int rank, num_ranks, i, iter, provided;
    
    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);

    double start_time, stop_time, elapsed_time;
    start_time = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

    srand(SEED * rank);

    flip = flip / num_ranks;

    for (int iter = 0; iter < flip; iter++) {
        x = (double)random() / (double)RAND_MAX;
        y = (double)random() / (double)RAND_MAX;
        z = sqrt((x*x) + (y*y));
        
        if (z <= 1.0) {
            local_count++;
        }
    }
    
    int global_count = 0;
    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    stop_time = MPI_Wtime();
    elapsed_time = stop_time - start_time;

    if (rank == 0) {
        pi = ((double)global_count / (double)(flip * num_ranks)) * 4.0;
        printf("pi: %f\n", pi);
        printf("Execution time: %f\n", elapsed_time);
    }

    MPI_Finalize();
    return 0;
}