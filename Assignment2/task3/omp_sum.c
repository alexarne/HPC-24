#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>
/*
Instructions: Implement a serial and a number of OpenMP parallel versions of serial_sum.
You can use the function omp_get_wtime() to get the time for measurement.
*/

const int NRUNS = 10;

double serial_sum(double *x, size_t size)
{
  double sum_val = 0.0;
  omp_set_num_threads(32);
  #pragma omp parallel for //reduction(+:sum_val)
  for (size_t i = 0; i < size; i++) {
    sum_val += x[i];
  }

  return sum_val;
}

void print_array(double *A, int rows, int cols){
    printf("[\n");
    for (int i = 0; i < rows; ++i){
        printf("[ ");
        for (int j = 0; j < cols; ++j){
            printf("%f ", A[i * cols + j]);
        }
        printf("]\n");
    }
    printf("]\n");
}

void generate_random(double *input, size_t size)
{
  for (size_t i = 0; i < size; i++) {
    input[i] = rand() / (double)(RAND_MAX);
  }
}

double calc_mean(double *x, size_t size){
  double sum = 0;
  for (size_t i = 0; i < size; ++i){sum += x[i];}
  return sum/size;
}

double calc_std(double *x, double mean, size_t size){

  double sum = 0;
  for (size_t i = 0; i < size; ++i){
    sum += pow(mean - x[i], 2);
  }
  return sqrt(sum/size);
}


int main(){

    size_t size = 1e7;
    double *A = malloc(size * sizeof(double));

    generate_random(A, size);

    double runtimes[NRUNS];
    
    for (size_t i = 0; i < NRUNS; ++i)
    {
      double start = omp_get_wtime();
      serial_sum(A, size);
      double stop = omp_get_wtime();
      runtimes[i] = (stop - start)*1e3;
    }

    double arraymean = calc_mean(A, size);
    double arraystd = calc_std(A, arraymean, size);

    printf("\nMean (array): %f\n", arraymean);
    printf("Standard deviation (array): %f\n\n", arraystd);

    double mean = calc_mean(runtimes, NRUNS);
    double std = calc_std(runtimes, mean, NRUNS);

    printf("Mean (time): %f ms\n", mean);
    printf("Standard deviation (time): %f ms\n\n", std);
    free(A);

    /*
    //The following code section (which should result in the sum 2001000) sometimes 
    //gives with the wrong result due to race conditions)
    size_t n = 2000;
    double *b = malloc(n*sizeof(double));
    for (size_t i = 0; i < n; ++i){
      b[i] = i+1;
    }
    double sum1 = serial_sum(b, n);
    printf("1 + 2 + 3 + 4 + ... + 2000 = %f\n", sum1);
    free(b);
    */
    return 0;
}