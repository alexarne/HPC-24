#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <string.h>
/*
Instructions: Implement a serial and a number of OpenMP parallel versions of serial_sum.
You can use the function omp_get_wtime() to get the time for measurement.
*/

const int NRUNS = 10;

double serial_sum(double *x, size_t size)
{
  double sum_val = 0.0;

  for (size_t i = 0; i < size; i++) {
    sum_val += x[i];
  }

  return sum_val;
}

double omp_sum(double *x, size_t size) {
  double sum_val = 0.0;

  #pragma omp parallel for reduction(+ : sum_val)
  for (size_t i = 0; i < size; i++) {
    sum_val += x[i];
  }
  
  return sum_val;
}

double omp_critical_sum(double *x, size_t size) {
  double sum_val = 0.0;

  #pragma omp parallel for reduction(+ : sum_val)
  for (size_t i = 0; i < size; i++) {
    #pragma omp critical
    {
      sum_val += x[i];
    }
  }

  return sum_val;
}

double omp_local_sum(double *x, size_t size) {
  int MAX_THREADS = 0;
  #pragma omp parallel reduction(+ : MAX_THREADS)
  MAX_THREADS += 1;
  //printf("Locally viewing number of threads as %d\n", MAX_THREADS);
  double local_sum[MAX_THREADS];
  memset(local_sum, 0, MAX_THREADS*sizeof(double));

  #pragma omp parallel for
  for (size_t i = 0; i < size; ++i) {
    int id = omp_get_thread_num();
    local_sum[id] += x[i];
    //printf("Thread %d accessing element %d\n", id, i);
  }

  //printf("Local_sum: ");
  double sum_val = 0;
  for (size_t i = 0; i < MAX_THREADS; ++i) {
    sum_val += local_sum[i];
    //printf("%f ", local_sum[i]);
  }
  //printf("\n");
  
  //printf("Local sum returning %f\n", sum_val);
  return sum_val;
}

double opt_local_sum(double *x, size_t size) {
  int MAX_THREADS = 0;
  #pragma omp parallel reduction(+ : MAX_THREADS)
  MAX_THREADS += 1;
  typedef struct { double val; char pad[128]; } tvals;
  tvals local_sum[MAX_THREADS];
  memset(local_sum, 0, MAX_THREADS*sizeof(tvals));

  #pragma omp parallel for
  for (size_t i = 0; i < size; ++i) {
    int id = omp_get_thread_num();
    local_sum[id].val += x[i];
  }

  double sum_val = 0;
  for (size_t i = 0; i < MAX_THREADS; ++i) {
    sum_val += local_sum[i].val;
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

    double runtimes_serial[NRUNS];  
    double runtimes_omp[NRUNS];  
    double runtimes_critical_omp[NRUNS];  
  
    for (size_t i = 0; i < NRUNS; ++i)
    {
      double start, stop, value1, value2, value3;
      
      start = omp_get_wtime();
      value1 = serial_sum(A, size);
      stop = omp_get_wtime();
      runtimes_serial[i] = (stop - start)*1e3;
      
      start = omp_get_wtime();
      value2 = omp_sum(A, size);
      stop = omp_get_wtime();
      runtimes_omp[i] = (stop - start)*1e3;
      
      start = omp_get_wtime();
      value3 = omp_critical_sum(A, size);
      stop = omp_get_wtime();
      runtimes_critical_omp[i] = (stop - start)*1e3;
    }

    double arraymean, arraystd;
    arraymean = calc_mean(A, size);
    arraystd = calc_std(A, arraymean, size);

    printf("\nMean (array): %f\n", arraymean);
    printf("Standard deviation (array): %f\n\n", arraystd);

    double mean, std;
    mean = calc_mean(runtimes_serial, NRUNS);
    std = calc_std(runtimes_serial, mean, NRUNS);
    printf("SERIAL:\n");
    printf("Mean (time): %f ms\n", mean);
    printf("Standard deviation (time): %f ms\n\n", std);
    
    mean = calc_mean(runtimes_omp, NRUNS);
    std = calc_std(runtimes_omp, mean, NRUNS);
    printf("OMP:\n");
    printf("Mean (time): %f ms\n", mean);
    printf("Standard deviation (time): %f ms\n\n", std);
    
    mean = calc_mean(runtimes_critical_omp, NRUNS);
    std = calc_std(runtimes_critical_omp, mean, NRUNS);
    printf("CRITICAL OMP:\n");
    printf("Mean (time): %f ms\n", mean);
    printf("Standard deviation (time): %f ms\n\n", std);

    int threads[] = {1, 2, 4, 8, 16, 20, 24, 28, 32};
    for (int i = 0; i < 9; ++i) {
      double runtimes[NRUNS];
      omp_set_num_threads(threads[i]);
      for (int j = 0; j < NRUNS; ++j) {
	double start = omp_get_wtime();
	omp_critical_sum(A, size);
	double stop = omp_get_wtime();
	runtimes[j] = (stop - start)*1e3; 
      }
      
      mean = calc_mean(runtimes, NRUNS);
      std = calc_std(runtimes, mean, NRUNS);
      printf("CRITICAL OMP (threads = %d):\n", threads[i]);
      printf("Mean (time): %f ms\n", mean);
      printf("Standard deviation (time): %f ms\n\n", std);
    }

    int threads2[] = {1, 32, 64, 128};
    for (int i = 0; i < 4; ++i) {
      double runtimes[NRUNS];
      omp_set_num_threads(threads2[i]);
      for (int j = 0; j < NRUNS; ++j) {
	double start = omp_get_wtime();
	omp_local_sum(A, size);
	double stop = omp_get_wtime();
	runtimes[j] = (stop - start)*1e3; 
      }
      
      mean = calc_mean(runtimes, NRUNS);
      std = calc_std(runtimes, mean, NRUNS);
      printf("OMP LOCAL (threads = %d):\n", threads2[i]);
      printf("Mean (time): %f ms\n", mean);
      printf("Standard deviation (time): %f ms\n\n", std);
    }
    
    for (int i = 0; i < 4; ++i) {
      double runtimes[NRUNS];
      omp_set_num_threads(threads2[i]);
      for (int j = 0; j < NRUNS; ++j) {
	double start = omp_get_wtime();
	opt_local_sum(A, size);
	double stop = omp_get_wtime();
	runtimes[j] = (stop - start)*1e3; 
      }
      
      mean = calc_mean(runtimes, NRUNS);
      std = calc_std(runtimes, mean, NRUNS);
      printf("OPT LOCAL (threads = %d):\n", threads2[i]);
      printf("Mean (time): %f ms\n", mean);
      printf("Standard deviation (time): %f ms\n\n", std);
    }
    
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
