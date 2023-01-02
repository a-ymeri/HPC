#include <stdio.h>
#include <math.h>

int chkPrime(int x) {
int TRUE=1;
int FALSE=0;
  int i;
  int foundFactor=FALSE;
  //printf("checking if %d is prime...\n",x);
  for (i=2; i<=sqrt((float) x) && foundFactor==FALSE; i++) {
    if (x%i == 0) {
      //  printf("%d has factor %d \n",x,i);
      foundFactor=TRUE;
    }
  }
 
  if (foundFactor) {
    return FALSE;
  }
  else { 
    return TRUE;
  }
}
