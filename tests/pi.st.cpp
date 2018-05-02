#include <stdio.h>
#include <math.h>


#include "../sched.h"
#include "utils/execution_timer.h"
unsigned num_threads = 4;

int count = 0;


class Argument {
public:
   StaticContext<long long> *ctx;
   unsigned threadId;
   double reduction;
   double step;
};

void* threadBody(void *arg) {
   Argument *threadArg = ((Argument*)arg);
   StaticContext<long long> *ctx = threadArg->ctx;
   unsigned threadId = threadArg->threadId;

   //BEGIN SHARED VARIABLES HERE
   double step = threadArg->step;
   //END SHARED VARIABLES HERE

  //UPDATE SETTINGS
   const long long iter_step = 1; //ctx->step=iter_step

   TaskEntry<long long> entry = ScheduleStaticEntry(ctx,threadId);
   for (long long iter_idx = entry.begin; iter_idx<entry.end; iter_idx += iter_step) {
  //COPY ITERATOR HERE
          long long i = iter_idx;
  //BEGIN KERNEL HERE
          double x = (i + 0.5)*step;
          threadArg->reduction += 4.0/(1.0 + x*x);
  //END KERNEL HERE
	}
   return NULL;
}

void run(long long n, int print){
  double pi;
  double step = 1.0/(double)n;

  double sum = 0;

  //#pragma omp parallel for reduction(+:sum)
  //for(long long i = 0; i < n; ++i) {
  //for(int n = 0; n<max_num; n++){ //skip all even numbers
  {
      unsigned nthreads = num_threads;
      long long begin = 0;
      long long end = n;
      long long nstep = 1;
      double reductionIdentity = 0;
      //StaticScheduler<long long> *sched = getStaticScheduler(nthreads,begin,end,nstep);
      StaticContext<long long> ctx;
      CreateStaticContext(&ctx, nthreads,begin,end,nstep);
      pthread_t threads[nthreads];
      Argument args[nthreads];
      for (unsigned threadId = 0; threadId<nthreads; threadId++) {
         args[threadId].ctx = &ctx;
         args[threadId].threadId = threadId;
         args[threadId].reduction = reductionIdentity;
         args[threadId].step = step;
         pthread_create(&threads[threadId],NULL,threadBody,static_cast<void*>(&args[threadId]));
      }
      for (unsigned threadId = 0; threadId<nthreads; threadId++) {
         pthread_join(threads[threadId],NULL);
         sum += args[threadId].reduction;
      }
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
