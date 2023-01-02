#include <stdio.h>
#include <math.h>
#include <mpi.h>

int chkPrime(int);

int TRUE=1;
int FALSE=0;
int maxInput=30000;
#define maxPEs 40


int main(int argc, char *argv[])
{
  int num;
  int i, j;
  int rank, size,numPrimes;
  int *globalNum;                   // integers to search
  int *p,*primes;              // dynamic memory for primes found
  int myRank;                         // MPI vars 
  double timer[2];

  MPI_Init(&argc, &argv);

  // only rank 0 to check parameters
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  if (myRank==0) {
    // malloc y to hold all numbers to check
    if (argc != 2 ) {
      printf("Usage: %s NUM\nto determine primes from 1 to NUM\n(where NUM is <%d)\n",argv[0],maxInput);
      MPI_Abort(MPI_COMM_WORLD, 101);
    }
    num = atoi(argv[1]);
    if (num>=maxInput) {
      printf("Usage: %s NUM\nto determine primes from 1 to NUM\n(where NUM is <%d)\n",argv[0],maxInput);
      MPI_Abort(MPI_COMM_WORLD, 102);
    }
    // start timer on rank 0
    timer[0] = MPI_Wtime();
  }
  // each MPI process will hold all numbers
  MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
  globalNum = (int *) malloc(num*sizeof(int));

  // only rank 0 sets initial values
  if (myRank==0) {
    for (i=0; i<num; i++) {
      globalNum[i]=i+1;          // array element [k] has value k+1 since k=0,1,2,...(NUM-1)
    }
  }

  // we now "broadcast" all data from rank 0 to all MPI ranks
  MPI_Bcast(globalNum, num, MPI_INT, 0, MPI_COMM_WORLD);
  
  // determine LOCAL info: 
  int numPEs;
  MPI_Comm_size(MPI_COMM_WORLD, &numPEs);
  int numEachProcess = num/numPEs;          // this may be different for each last rank
  int myStart = myRank*numEachProcess;
  if (myRank==0) {
    // skip i=0 since globalNum[0] is 1 
    // so start at i=1 _and_ do 1 less item
    myStart = 1;
    numEachProcess--;
  } else if (myRank==numPEs-1) {
    // ensure all points covered
    numEachProcess=(num-myStart);
  }
  
  printf("Rank %d starts at %d doing %d\n", myRank, myStart, numEachProcess);

  // malloc space for estimated number of max primes in region
  numPrimes = (numEachProcess/2)+2;                        // so we are also dividing up this memory usage over MPI processes
  primes = (int *) malloc(numPrimes*sizeof(int));

  // find primes
  j = 0;   // counter of num primes found
  for (i=; i<; i++) {        // each MPI process does a sub-range of the full range ==> dividing up the work
    int val = globalNum[i];
    if(chkPrime(val)==TRUE) {
      primes[j] = val;
      j++;
      if (j>numPrimes) {
	printf("%d: insufficient memory after %d prime\n", myRank, j);
	break;
      }
    }
  }

  printf("Rank %d: Total primes found = %d\n", myRank, j);

  for (i=0; i<j; i++) {
    printf("%d: %i   ", myRank, primes[i]);
  }
  printf("\n");

    /* for this example we only want to 'gather' the number of primes per rank */
  // use MPI collective to gather info. 
  int primesPerRank[numPEs];
  MPI_Gather(??, ??, ??, primesPerRank, ??, ??, 0, MPI_COMM_WOLRD) // you need to complete this call and ensure you understand
  if (myRank==0) {
    // finish timers on rank 0
    timer[1] = MPI_Wtime();
    int sum=0;
    for (i=0; i<numPEs; i++) {
      printf("Rank %d had %d primes\n", i, primesPerRank[i]);
      sum += primesPerRank[i];
    }
    printf("Total #primes found: %d in %g seconds\n", sum, timer[1]-timer[0]);
  }
  // finialise
  MPI_Finalize();
  return 0;
}


