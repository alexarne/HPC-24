/*
    Exercise 1: OpenMP Hello World
*/

#include <omp.h>
#include <stdio.h>

int main(){

    omp_set_num_threads(4);
    #pragma omp parallel // Fork into parallel environment
    {
        int i = omp_get_thread_num();
        printf("Hello World from Thread %d!\n", i);
    }
    return 0;
}