#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
 
int main(int argc, char *argv[]) {
  // initialize MPI
  MPI_Init(&argc, &argv);

  // get the number of processes
  int num_procs;
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  // get the rank of the current process
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // read in the size of the array from the command line
  int m = atoi(argv[1]);
  int n = atoi(argv[2]);
  int p = atoi(argv[3]);
  int matrix_size = m * n;
  int matrix_size_b = n * p;

  // allocate memory for the array
  int *data = NULL;
  if (rank == 0) {
    data = (int *)malloc(matrix_size * sizeof(int));
    // initialize the array
    for (int i = 0; i < matrix_size; i++) {
      data[i] = i;
    }
  }

    int *data_b = NULL;
    data_b = (int *)malloc(matrix_size_b * sizeof(int));
    // initialize the array
    for (int i = 0; i < matrix_size_b; i++) {
        data_b[i] = i;
    }

  // divide the array evenly among the processes
    int rows_per_proc = m / num_procs;
    int remainder = m % num_procs;

    int *send_counts = NULL;
    send_counts = (int *)malloc(num_procs * sizeof(int));

    int *displs = NULL;
    displs = (int *)malloc(num_procs * sizeof(int));

    for (int i = 0; i < num_procs; i++) {
        send_counts[i] = rows_per_proc * n;
        if (i < remainder) {
            send_counts[i] += n;
        }
    }




    for (int i = 0; i < num_procs; i++) {
        displs[i] = 0;
        if (i > 0) {
            displs[i] = displs[i-1] + send_counts[i-1];
        }
    }
    

    
    // allocate memory for the chunk
    int *chunk = (int *)malloc(send_counts[rank] * n * sizeof(int));

    // scatter the array to the processes

    MPI_Scatterv(data, send_counts, displs, MPI_INT, chunk, send_counts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    /* SCATTER OVER THIS   -   OPERATION PHASE */

    int local_sum;

    int rows = send_counts[rank] / n;
    int chunk_c_size = rows * p;
    int *chunk_c = (int *)malloc(chunk_c_size * sizeof(int));

    printf("chunk c size: %d \n", chunk_c_size);
    //loop for rows (each process gets I rows)
    for (int i = 0; i < rows; i++) {
        //loop for columns 
        for (int j = 0; j<p; j++){
            local_sum = 0;
            //loop for summatiom
            for (int k = 0; k<n; k++){
                local_sum += chunk[i*n + k] * data_b[k*p + j];
            }
            // printf("chunk_c[%d] = %d \n", i*p + j, local_sum);
            chunk_c[i*p + j] = local_sum;
        }
    }
    




    // // gather the array back to rank 0
    int *final_data = NULL;
    if (rank == 0) {
        final_data = (int *)malloc(m * p * sizeof(int));
    }


    int *recv_counts = NULL;
    recv_counts = (int *)malloc(num_procs * sizeof(int));

    for (int i = 0; i < num_procs; i++) {
        recv_counts[i] = chunk_c_size;
    }


    for (int i = 0; i < num_procs; i++) {
        displs[i] = 0;
        if (i > 0) {
            displs[i] = displs[i - 1] + recv_counts[i - 1];
        }
    }

    MPI_Gatherv(chunk_c, recv_counts[rank] , MPI_INT, final_data, recv_counts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    // print the final array on rank a0
    if (rank == 0) {
        for (int i = 0; i < m*p; i++) {
            printf("final_data[%d] = %d \n", i, final_data[i]);
        }
    }

// clean up memory and finalize MPI
// free(chunk);
// if (rank == 0) {
// free(data);
// // free(final_data);
// // free(recv_counts);
// free(displs);
// }
MPI_Finalize();

return 0;
}