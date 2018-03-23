#include <stdio.h>
#include <math.h>


int count = 0;

void run(long long n, int print){
  double pi;
  double step = 1.0/(double)n;

  double sum = 0;

#ifdef USE_OMP_STATIC
  #pragma omp parallel for reduction(+:sum) num_threads(8) schedule(static)
#else
  #pragma omp parallel for reduction(+:sum) num_threads(8) schedule(dynamic,64)
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
   long long n = 100000L;
   for(int i = 0; i<1000; i++){
      run(n,1);
   }
   run(n,1);
   printf("Count: %d\n",count);
   return 0;
}

