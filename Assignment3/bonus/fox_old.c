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

  double start_time;
  srand(MPI_Wtime());
  int num_ranks, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Set up communication via rows and columns
  MPI_Comm comm_cart;
  int p = sqrt(num_ranks);
  int dims[2] = { p, p };
  int n = 144;
  int block_size = n / p;
  if (rank == 0) {
    printf("Processes: %i\n", p);
    printf("Block size: %i\n", block_size);
    printf("n: %i\n\n", n);
  }
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
  for (int i = 0; i < block_size*block_size; ++i) c[i] = 0;
  double** C_true;
  
  if (rank == 0) {
    double A[n][n];
    double B[n][n];
    
    // Initialize matrices
    for (int i = 0; i < p; ++i) {
      for (int j = 0; j < p; ++j) {
        for (int y = 0; y < block_size; ++y) {
          for (int x = 0; x < block_size; ++x) {
              A[i * block_size + y][j * block_size + x] = rand() / (double)(RAND_MAX);
              B[i * block_size + y][j * block_size + x] = rand() / (double)(RAND_MAX);
          }
        }
      }
    }

    // Verification
    C_true = (double**) malloc(n*sizeof(double*));
    for (int i = 0; i < n; ++i) C_true[i] = (double*) malloc(n*sizeof(double)); 
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        C_true[i][j] = 0;
        for (int k = 0; k < n; ++k) {
          C_true[i][j] += A[i][k] * B[k][j];
        }
      }
    }

    start_time = MPI_Wtime();
    
    // Distribute elements to processes
    MPI_Request requests[2 * (p * p - 1)];
    for (int i = 0; i < p; ++i) {
      for (int j = 0; j < p; ++j) {
        double A_buffer[block_size * block_size];
        double B_buffer[block_size * block_size];
        for (int y = 0; y < block_size; ++y) {
          for (int x = 0; x < block_size; ++x) {
              A_buffer[y * block_size + x] = A[i * block_size + y][j * block_size + x];
              B_buffer[y * block_size + x] = B[i * block_size + y][j * block_size + x];
          }
        }
        if (i != 0 || j != 0) {
          int receiver = i * p + j;
          MPI_Send(A_buffer, block_size * block_size, MPI_DOUBLE, receiver, 10, MPI_COMM_WORLD);
          MPI_Send(B_buffer, block_size * block_size, MPI_DOUBLE, receiver, 20, MPI_COMM_WORLD);
	  /**
	  printf("sending A to process %i:\n", receiver);
	  for (int i = 0; i < n; ++i) {
	    for (int j = 0; j < n; ++j) {
	      printf("%f ", A[i][j]);
	    }
	    printf("\n");
	  }
	  printf("sending B to process %i:\n", receiver);
	  for (int i = 0; i < n; ++i) {
	    for (int j = 0; j < n; ++j) {
	      printf("%f ", B[i][j]);
	    }
	    printf("\n");
	    }*/
        } else {
          for (int x = 0; x < block_size * block_size; ++x) {
              a[x] = A_buffer[x];
              b[x] = B_buffer[x];
          }
        }
      }
    }
    //MPI_Waitall(2 * (p * p - 1), requests, MPI_STATUSES_IGNORE);
    /**
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
    */
  } else {
    MPI_Status stat;
    MPI_Request r1, r2;
    MPI_Recv(a, block_size*block_size, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD, &stat);
    MPI_Recv(b, block_size*block_size, MPI_DOUBLE, 0, 20, MPI_COMM_WORLD, &stat);
  }
  /**
  printf("process %i received a: ", rank);
  for (int i = 0; i < block_size*block_size; ++i) printf("%f ", a[i]);
  printf("\n");
  printf("process %i received b: ", rank);
  for (int i = 0; i < block_size*block_size; ++i) printf("%f ", b[i]);
  printf("\n");
  */
  // Do the algorithm p iterations (sqrt of num_processes)
  for (int i = 0; i < p; ++i) {
    int col_rank, row_rank;
    MPI_Comm_rank(comm_cols, &col_rank);
    MPI_Comm_rank(comm_rows, &row_rank);
    
    // Broadcast the diagonal+i for rows
    double a_broadcast[block_size*block_size];
    int root = (col_rank+i)%p;
    if (row_rank == root) {
      //printf("iter %i row broadcasting from process %i\n", i, rank);
      //printf("process %i has row_rank %i and sought root is %i\n", rank, row_rank, root);
      MPI_Bcast(a, block_size*block_size, MPI_DOUBLE, root, comm_rows);
      for (int j = 0; j < block_size*block_size; ++j) a_broadcast[j] = a[j];
    } else {
      MPI_Bcast(a_broadcast, block_size*block_size, MPI_DOUBLE, root, comm_rows);
    }

    // Do matmul and save in c
    for (int y = 0; y < block_size; ++y) {
      for (int x = 0; x < block_size; ++x) {
	for (int k = 0; k < block_size; ++k) {
	  c[y*block_size + x] += a_broadcast[y*block_size + k] * b[k*block_size + x];
	}
      }
    }
    //printf("process %i calculated c: ", rank);
    //for (int j = 0; j < block_size*block_size; ++j) printf("%f ", c[j]);
    //printf("\n");
    
    // Roll b upwards
    //printf("process %i is col rank %i\n", rank, col_rank);
    MPI_Status stat;
    MPI_Send(b, block_size*block_size, MPI_DOUBLE, (col_rank-1+p)%p, 20, comm_cols);
    MPI_Recv(b, block_size*block_size, MPI_DOUBLE, (col_rank+1)%p, 20, comm_cols, &stat);
  
    
  }

  if (rank == 0) {
    // Collect all c:s and combine into final matrix
    double C[n][n];
    for (int i = 0; i < block_size; ++i) {
      for (int j = 0; j < block_size; ++j) {
	C[i][j] = c[i*block_size+j];
      }
    }
    for (int k = 1; k < p*p; ++k) {
      MPI_Status stat;
      MPI_Recv(c, block_size*block_size, MPI_DOUBLE, k, 30, MPI_COMM_WORLD, &stat);
      for (int i = 0; i < block_size; ++i) {
	for (int j = 0; j < block_size; ++j) {
	  C[i+(k/p)*block_size][j+(k%p)*block_size] = c[i*block_size+j];
	}
      }
    }

    double elapsed_time = MPI_Wtime() - start_time;
    printf("Time elapsed: %.10f seconds\n", elapsed_time);
    
    int correct = 1;
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
	if (fabs(C[i][j] - C_true[i][j]) > 1e-10) correct = 0;
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
    } else {
      printf("Calculation is correct!\n");
    }
    

    printf("Error (Frobenius norm): ");
    double frob = 0;
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
	double delta = C_true[i][j]-C[i][j];
	frob += delta*delta;
      }
    }
    frob = sqrt(frob);
    printf("%.16f\n", frob);
    
    for (int i = 0; i < n; ++i) free(C_true[i]);
    free(C_true);
  } else {
    // Send all c:s
    MPI_Request req;
    MPI_Send(c, block_size*block_size, MPI_DOUBLE, 0, 30, MPI_COMM_WORLD);
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
