#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);

  double start_time = MPI_Wtime();
  srand(start_time);
  int num_ranks, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Set up communication via rows and columns
  MPI_Comm comm_cart;
  int p = sqrt(num_ranks);
  int dims[2] = { p, p };
  int block_size = 2;
  int n = p * block_size;
  int periods[2] = { 1, 1 };
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &comm_cart);

  MPI_Comm comm_rows;
  int dims_keep_rows[2] = { 0, 1 };
  MPI_Cart_sub(comm_cart, dims_keep_rows, &comm_rows);
  MPI_Comm comm_cols;
  int dims_keep_cols[2] = { 1, 0 };
  MPI_Cart_sub(comm_cart, dims_keep_cols, &comm_cols);

  // Local variables for this process's block/tile
  double a[block_size * block_size];
  double b[block_size * block_size];
  double c[block_size * block_size];

  if (rank == 0) {
    double A[n][n];
    double B[n][n];
    double C[n][n];

    // Initialize and distribute all elements
    MPI_Request requests[2 * (p * p - 1)];
    for (int i = 0; i < p; ++i) {
      for (int j = 0; j < p; ++j) {
        double A_buffer[block_size * block_size];
        double B_buffer[block_size * block_size];
        for (int y = 0; y < block_size; ++y) {
          for (int x = 0; x < block_size; ++x) {
              A[i * block_size + y][j * block_size + x] = rand() / (double)(RAND_MAX);
              B[i * block_size + y][j * block_size + x] = rand() / (double)(RAND_MAX);
              A_buffer[y * block_size + x] = A[i * block_size + y][j * block_size + x];
              B_buffer[y * block_size + x] = B[i * block_size + y][j * block_size + x];
          }
        }
        if (i != 0 || j != 0) {
          int receiver = i * p + j;
          MPI_Isend(A_buffer, block_size * block_size, MPI_INT, receiver, 10, MPI_COMM_WORLD, &requests[2 * (receiver - 1)]);
          MPI_Isend(B_buffer, block_size * block_size, MPI_INT, receiver, 20, MPI_COMM_WORLD, &requests[2 * (receiver - 1) + 1]);
        } else {
          for (int x = 0; x < block_size * block_size; ++x) {
              a[x] = A_buffer[x];
              b[x] = B_buffer[x];
          }
        }
      }
    }
    MPI_Waitall(2 * (p * p - 1), requests, MPI_STATUSES_IGNORE);

    printf("A:\n");
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        printf("%f ", A[i][j]);
      }
      printf("\n");
    }
    printf("B:\n");
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        printf("%f ", B[i][j]);
      }
      printf("\n");
    }

    // Verification
    double C_true[n][n];
    int correct = 1;
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        C_true[i][j] = 0;
        for (int k = 0; k < n; ++k) {
          C_true[i][j] += A[i][k] * B[k][j];
        }
        if (C[i][j] != C_true[i][j]) {
          correct = 0;
        }
      }
    }
    
    if (correct != 1) {
      printf("incorrect result\nexpected:\n");
      for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
          printf("%f ", C_true[i][j]);
        }
        printf("\n");
      }
      printf("\nreceived:\n");
      for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
          printf("%f ", C[i][j]);
        }
        printf("\n");
      }
    }
    } else {
      MPI_Status stat;
      MPI_Request r1, r2;
      MPI_Recv(a, block_size*block_size, MPI_INT, 0, 10, MPI_COMM_WORLD, &stat);
      MPI_Recv(b, block_size*block_size, MPI_INT, 0, 20, MPI_COMM_WORLD, &stat);
      printf("process %i received a: ", rank);
      for (int i = 0; i < block_size*block_size; ++i) printf("%f ", a[i]);
      printf("\n");
      printf("process %i received b: ", rank);
      for (int i = 0; i < block_size*block_size; ++i) printf("%f ", b[i]);
      printf("\n");
      
    }

    /**
    int test = 0, root = 0;
    if (rank % 3 == 0) {
      test = rank * 100 + 100;
    }
    MPI_Bcast(&test, 1, MPI_INT, root, comm_rows);
    printf("process %i reporting test as %i\n", rank, test);
    */
    
    MPI_Finalize();
    return 0;
}
