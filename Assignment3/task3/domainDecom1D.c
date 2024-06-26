
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX(a,b) (((a)>(b))?(a):(b))

int main(int argc, char *argv[]){
    int rank, size, i, provided;
    
    // number of cells (global)
    int nxc = 128; // make sure nxc is divisible by size
    double L = 2*3.1415; // Length of the domain
    

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // number of nodes (local to the process): 0 and nxn_loc-1 are ghost cells 
    int nxn_loc = nxc/size + 3; // number of nodes is number cells + 1; we add also 2 ghost cells
    double L_loc = L/((double) size);
    double dx = L / ((double) nxc);
    
    // define out function
    double *f = calloc(nxn_loc, sizeof(double)); // allocate and fill with z
    double *dfdx = calloc(nxn_loc, sizeof(double)); // allocate and fill with z

    for (i=1; i<(nxn_loc-1); i++)
      f[i] = sin(L_loc*rank + (i-1) * dx);
    
    // need to communicate and fill ghost cells f[0] and f[nxn_loc-1]
    // communicate ghost cells
    // ...
    // ...  
    
    MPI_Status  status[4];
    int prev = (rank - 1 + size) % size;
    int next = (rank + 1) % size;

    #ifdef BLOCKING
      MPI_Send(&f[1], 1, MPI_DOUBLE, prev, 0, MPI_COMM_WORLD);
      MPI_Send(&f[nxn_loc - 2], 1, MPI_DOUBLE, next, 0, MPI_COMM_WORLD);
      MPI_Recv(&f[nxn_loc - 1], 1, MPI_DOUBLE, next, 0, MPI_COMM_WORLD, &status[0]);
      MPI_Recv(&f[0], 1, MPI_DOUBLE, prev, 0, MPI_COMM_WORLD, &status[1]);
    #else
      MPI_Request requests[4];

      MPI_Isend(&f[1], 1, MPI_DOUBLE, prev, 0, MPI_COMM_WORLD, &requests[0]);
      MPI_Irecv(&f[0], 1, MPI_DOUBLE, prev, 0, MPI_COMM_WORLD, &requests[1]);
      MPI_Isend(&f[nxn_loc - 2], 1, MPI_DOUBLE, next, 0, MPI_COMM_WORLD,  &requests[2]);
      MPI_Irecv(&f[nxn_loc - 1], 1, MPI_DOUBLE, next, 0, MPI_COMM_WORLD,  &requests[3]);
      MPI_Waitall(4, requests, status);
    #endif

    // here we finish the calculations

    // calculate first order derivative using central difference
    // here we need to correct value of the ghost cells!
    for (i=1; i<(nxn_loc-1); i++)
      dfdx[i] = (f[i+1] - f[i-1])/(2*dx);

    double max_err = 0.0;
    for(i=1; i < (nxn_loc - 1); i++)
      max_err = MAX(max_err, abs(cos(L_loc*rank + (i-1) * dx) - dfdx[i]));
    
    #ifdef BLOCKING 
      printf("rank %d max error: %f (BLOCKING)\n", rank, max_err);
    #else 
      printf("rank %d max error: %f\n", rank, max_err);
    #endif

    MPI_Finalize();
}
