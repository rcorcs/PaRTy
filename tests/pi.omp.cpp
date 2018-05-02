#include <stdio.h>
#include <math.h>

//#include "../sched.h"
#include "utils/execution_timer.h"
unsigned num_threads = 4;

int count = 0;


void run(long long n, int print){
  double pi;
  double step = 1.0/(double)n;

  double sum = 0;

#ifdef USE_OMP_STATIC
  #pragma omp parallel for reduction(+:sum) num_threads(num_threads) schedule(static)
#else
  #pragma omp parallel for reduction(+:sum) num_threads(num_threads) schedule(dynamic,64)
#endif
  for(long long i = 0; i < n; ++i) {
     double x = (i + 0.5)*step;
     sum += 4.0/(1.0 + x*x);;
  }
  pi = sum*step;

  if (print) {
    printf("Pi: %f\n", pi);
  }
  count++;
}

int main(){

   __timer_prologue();

   long long n = 100000L;
   for(int i = 0; i<100; i++){
      run(n,1);
   }
   run(n,1);
   __timer_epilogue();
   printf("Count: %d\n",count);
   return 0;
}
/*
int main(){
   long long n = 500000000L;
   for(int i = 0; i<20; i++){
      run(n,1);
   }
   run(n,1);
   printf("Count: %d\n",count);
   return 0;
}
*/
