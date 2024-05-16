#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void) {
  printf("%f\n", omp_get_wtime());
}
