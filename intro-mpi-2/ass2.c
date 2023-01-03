#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {


    //read a.dat

    FILE *fp;
    fp = fopen("a.dat", "r");
    int m, n;

    //first line of a.dat is n and m
    fscanf(fp, "%d %d", &m, &n);

    // second line of a.dat is the array
    float *A = NULL;
    A = (float *)malloc(sizeof(float) * m * n);
    for (int i = 0; i < n*m; i++) {
        fscanf(fp, "%f", &A[i]);
    }

    //read b.dat
    fp = fopen("b.dat", "r");
    int n2, p;


    //first line of b.dat is n and p
    fscanf(fp, "%d %d", &n2, &p);

    if (n != n2) {
        printf("Error: n != n2");
        return 1;
    }


    // second line of b.dat is the array
    printf("%d %d ", n, p)
    float *B = NULL;
    B = (float *)malloc(sizeof(float) * n * p);

    printf(n*p)
    for (int i = 0; i < n*p; i++) {
        fscanf(fp, "%f", &B[i]);
    }






    // Initialize MPI
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    //use malloc to allocate memory for local_C
    float *local_C = (float *) malloc(sizeof(float) * m * p);

for (int i = 0; i < m*p; i++) {
    local_C[i] = 0;
}
    //A is mxn, B is nxp, C is mxp

    for (int i = rank; i < m; i += size) {
        for (int j = 0; j < p; j++) {

            float sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i*n + k] * B[k*p + j];
            }
            local_C[i*p + j] = sum;
        }
    }

    // put local_C into new matrix C
    float *C = (float *)malloc(sizeof(float) * m * p);

    // Declare an array to store the number of elements contributed by each process
    int sendcounts[size];

    // Declare an array to store the offsets of the gathered array for each process
    int displs[size];

    // Calculate the number of rows of C contributed by each process
    for (int i = 0; i < size; i++) {
        sendcounts[i] = (m / size) * p;
    }

    // Calculate the offsets of the gathered array for each process
    displs[0] = 0;
    for (int i = 1; i < size; i++) {
        displs[i] = displs[i - 1] + sendcounts[i - 1];
    }

    // Gather the local_C arrays into C
    MPI_Gatherv(local_C, m*p, MPI_FLOAT, C, sendcounts, displs, MPI_FLOAT, 0, MPI_COMM_WORLD);
    //print C
    if (rank == 0) {
        printf("%d %d ", m, p);
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < p; j++) {
                printf("%f ", C[i*p + j]);
            }
                            printf("\n");

        }
        printf(" ");
    }


    //read c.dat using malloc


    //compare C to c.dat
    // if (rank == 0) {
    //     fp = fopen("c.dat", "r");
    //     int m2, p2;

    //     //first line of c.dat is m and p
    //     fscanf(fp, "%d %d", &m2, &p2);

    //     if (m != m2 || p != p2) {
    //         printf("Error: m != m2 || p != p2");
    //         return 1;
    //     }

    //     // second line of c.dat is the array
    //     float C2[m*p];

    //     for (int i = 0; i < m*p; i++) {
    //         fscanf(fp, "%f", &C2[i]);
    //     }

    //     for (int i = 0; i < m*p; i++) {
    //         if (C[i] != C2[i]) {
    //             printf("Error: C[i] != C2[i]");
    //             return 1;
    //         }
    //     }
    // }

    // Finalize MPI
    MPI_Finalize();
}