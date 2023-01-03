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
  int n = atoi(argv[1]);

  // allocate memory for the array
  int *data = NULL;
  if (rank == 0) {
    data = (int *)malloc(n * sizeof(int));
    // initialize the array
    for (int i = 0; i < n; i++) {
      data[i] = i;
    }
  }

  // divide the array evenly among the processes
  int items_per_proc = n / num_procs;
  int remainder = n % num_procs;

    int *send_counts = NULL;
    send_counts = (int *)malloc(num_procs * sizeof(int));

    int *displs = NULL;
    displs = (int *)malloc(num_procs * sizeof(int));

    for (int i = 0; i < num_procs; i++) {
        send_counts[i] = items_per_proc;
        if (i < remainder) {
            send_counts[i]++;
        }
    }




    for (int i = 0; i < num_procs; i++) {
        displs[i] = 0;
        if (i > 0) {
            displs[i] = displs[i-1] + send_counts[i-1];
        }
    }

    for (int i = 0; i < num_procs; i++) {
        if(i != num_procs - 1){
            send_counts[i] = send_counts[i] + 2;
        }
    }

    //print send_counts
    if (rank == 0){
    for (int i = 0; i < num_procs; i++) {
        printf("send_counts[%d] = %d", i, send_counts[i]);
    }
    }
    
    // send_counts[1] = 5-rank;
    // send_counts[2] = 5-rank;

    
    // allocate memory for the chunk
    int *chunk = (int *)malloc(send_counts[rank] * sizeof(int));

    // scatter the array to the processes

    MPI_Scatterv(data, send_counts, displs, MPI_INT, chunk, send_counts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    // print the chunk on each process
    for (int i = 0; i < send_counts[rank]; i++) {
        printf("rank %d: %d \n", rank, chunk[i]);
    }
    // multiply each element by 2
    for (int i = 0; i < send_counts[rank]; i++) {
        chunk[i] *= 2;
    }

    // gather the array back to rank 0
    int *final_data = NULL;
    if (rank == 0) {
        final_data = (int *)malloc(n * sizeof(int));
    }


    int *recv_counts = NULL;
    recv_counts = (int *)malloc(num_procs * sizeof(int));

    for (int i = 0; i < num_procs; i++) {
        recv_counts[i] = send_counts[i];
    }

    // MPI_Gatherv(chunk, send_counts[rank], MPI_INT, final_data, send_counts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    // // print the final array on rank 0
    // if (rank == 0) {
    //     for (int i = 0; i < n; i++) {
    //         printf("final_data[%d] = %d \n", i, final_data[i]);
    //     }
    // }

// clean up memory and finalize MPI
free(chunk);
if (rank == 0) {
free(data);
// free(final_data);
// free(recv_counts);
free(displs);
}
MPI_Finalize();

return 0;
}