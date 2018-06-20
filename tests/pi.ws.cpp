#include <stdio.h>
#include <math.h>

#include "../sched.h"
#include "../execution_timer.h"
unsigned num_threads = 4;



int count = 0;

class Argument {
public:
   ChunkedWSContext<long long> *ctx;
   unsigned threadId;
   double reduction;
   double step;
};

CREATE_CHUNKED_WS_THREAD(threadBody, Argument, 1, 512, long long, i,
   double step = ThreadArgs->step;
, 
	double x = (i + 0.5)*step;
   ThreadArgs->reduction += 4.0/(1.0 + x*x);
)

void run(long long n, int print){
  double pi;
  double step = 1.0/(double)n;

  double sum = 0;

  LAUNCH_CHUNKED_WS_THREADS(num_threads, long long, 0, n, 1, 512, Argument, threadBody, 
         args[threadId].reduction = 0;
         args[threadId].step = step;
  ,
         sum += args[threadId].reduction;
  )

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
