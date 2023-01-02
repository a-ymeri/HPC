#include <stdio.h>
#include <math.h>

int chkPrime(int);

int maxInput=30000;

int main(int argc, char *argv[])
{
int TRUE=1;
int FALSE=0;
  int num;
  int i, j;
  int rank, size,numPrimes;
  int *globalNum;                   // integers to search
  int *p,*primes;              // dynamic memory for primes found

  // malloc y to hold all numbers to check
  if (argc != 2 ) {
    printf("Usage: %s NUM\nto determine primes from 1 to NUM\n(where NUM is <%d)\n",argv[0],maxInput);
    return -1;
  }
  num = atoi(argv[1]);
  if (num>=maxInput) {
    printf("Usage: %s NUM\nto determine primes from 1 to NUM\n(where NUM is <%d)\n",argv[0],maxInput);
    return -1;
  }
  globalNum = (int *) malloc(num*sizeof(int));
  for (i=0; i<num; i++) {
    globalNum[i]=i+1;          // array element [k] has value k+1 since k=0,1,2,...(NUM-1)
  }

  // malloc space for estimated number of max primes in region
  numPrimes = (num/2)+2;
  primes = (int *) malloc(numPrimes*sizeof(int));

  // find primes
  j = 0;   // counter of num primes found
  // we don't need check globalNum[0] which has value 1
  for (i=1; i<num; i++) {
    int val = globalNum[i];
    //  printf("checking %d\n", val);
    if(chkPrime(val)==TRUE) {
      //  printf("found %d to be PRIME\n", val);
      primes[j] = val;
      j++;
      //  printf("j=%d\n", j);
      if (j>numPrimes) {
	printf("insufficient memory after %d prime\n", j);
	break;
      }
    }
  }

  printf("Total primes found = %d\n", j);
  for (i=0; i<j; i++) {
    printf("%i   ", primes[i]);
  }
  printf("\n");

  return 0;
}


