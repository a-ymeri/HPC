/*
 * COMP328: MPI solution for quadrature
 * integral = sum of areas trapezoidals that approximate curve
 * area of trapezoidal = mean height * width
 *
 * (c) m.k.bane@liverpool.ac.uk
 */

#include <stdio.h>
#include <math.h>

#include <sys/time.h>  // for wallclock timing functions
#include <mpi.h>       // header for MPI 

double func(double);   // prototype for func to integrate (separate file)

int main(void) {
  const double a=0.1, b=200.0;  /* bounds */
  const int numQuads = 30000000;
  // const int numQuads = 10;
  double localSum, width, x, y, meanHeight;
  int i;              /* loop counter */

  /* for non-MPI timing */
  struct timeval timer[4];

  /* vars for MPI */
  int numProcesses, rankNum;
  double MPI_timer[4];  // note that this array is private to _each_ MPI process

  MPI_Init(NULL, NULL);

  gettimeofday(&timer[0], NULL); 
  MPI_timer[0] = MPI_Wtime();

  MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
  MPI_Comm_rank(MPI_COMM_WORLD, &rankNum);

  width = (b-a) / (float) numQuads;
  localSum = 0.0;


  /* split iterations over MPI processes */
  int numQuads_perProcess = numQuads/numProcesses;
  int start_perProcess = rankNum*numQuads_perProcess;
  if (rankNum == numProcesses-1) numQuads_perProcess = numQuads - (numQuads_perProcess*rankNum);
  int finish_perProcess = start_perProcess + numQuads_perProcess - 1;
  // debugging: check cover full range
  //printf("rank %d/%d nQ_perP=%d\n", rankNum, numProcesses, numQuads_perProcess);
  //printf("rank %d/%d start=%d finish=%d\n", rankNum, numProcesses, start_perProcess, finish_perProcess);
   
  gettimeofday(&timer[1], NULL); 
  MPI_timer[1] = MPI_Wtime();

  /* by having different sub-ranges of the iteration space, 
     we have divided the work between the MPI processes */
  for (i=start_perProcess; i<=finish_perProcess; i++) {
    x = a + i*width;  // left of trapezoidal
    y = x + width;    // right of trapezoidal
    meanHeight = 0.5 * (func(x) + func(y));
    localSum += meanHeight*width;
    // debugging: next line will show localSum increasing per MPI process
    // printf("rank-%d iter-%d %.15f %f %f %f\n",rankNum,i,x,y,meanHeight,localSum);
  }
  // debugging: next line will show the accumulate localSum per MPI process
  // printf("MPI partial sum on %d rank of %d processes - integral = %f\n", rankNum, numProcesses, localSum);
  gettimeofday(&timer[2], NULL); 
  MPI_timer[2] = MPI_Wtime();


  /* now we have to sum all 'localSum' from each rank in to a global "sum"
     We naively do this by each rank>0 sending its value to rank=0 */
  double sum;
  int tag = 999;
  if (rankNum>0) {
    MPI_Send(&localSum, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
  }
  else {
    sum = localSum;           // i.e. rank=0 contribution
    double inputVal;          // buffer to receive data
    for (i=1; i<numProcesses; i++) {
      MPI_Recv(&inputVal, 1, MPI_DOUBLE, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      sum += inputVal;
    }
  }

  gettimeofday(&timer[3], NULL); 
  MPI_timer[3] = MPI_Wtime();


  /* for non-MPI timers, we have the following timing regions:
     timer[3] - timer[0] => time for whole code
     timer[2] - timer[1] => time for the parallelised loop

     NB we only need to output from a single MPI process
  */

  if (rankNum == 0) {
    //printf("MPI sum across %d processes - integral = %f\n", numProcesses, sum);

    /* whole code time */
    double wallSecs = (timer[3].tv_sec - timer[0].tv_sec);           
    double WALLtimeTaken = 1.0E-06 * ((wallSecs*1000000) + (timer[3].tv_usec - timer[0].tv_usec));
    //printf("WALL CLOCK Time (whole code): %f seconds  \n", WALLtimeTaken);

    /* whole code time */
    wallSecs = (timer[2].tv_sec - timer[1].tv_sec);           
    WALLtimeTaken = 1.0E-06 * ((wallSecs*1000000) + (timer[2].tv_usec - timer[1].tv_usec));
    //printf("WALL CLOCK Time (loop only):  %f seconds  \n", WALLtimeTaken);
  }


  /* we also have timings on each MPI process but we need to be careful how to output */
  for (i=0; i<numProcesses; i++) {
    if(rankNum==i) {
      printf("rank %d: whole code: %f seconds (of which loop took %f seconds)\n", rankNum, MPI_timer[3]-MPI_timer[0], MPI_timer[2]-MPI_timer[1]);
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }

  MPI_Finalize();
}
