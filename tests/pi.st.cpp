#include <stdio.h>
#include <math.h>


#include "../sched.h"

int count = 0;

unsigned num_threads = 8;

class Argument {
public:
   StaticScheduler<long long> *sched;
   unsigned threadId;
   double reduction;
   double step;
};

void* threadBody(void *arg) {
   Argument *threadArg = ((Argument*)arg);
   StaticScheduler<long long> *sched = threadArg->sched;
   unsigned threadId = threadArg->threadId;
   double step = threadArg->step;
   TaskEntry<long long> entry = sched->get(threadId);
   long long nstep = sched->getStep();
   for (long i = entry.begin; i<entry.end; i += nstep) {
		double x = (i + 0.5)*step;
      threadArg->reduction += 4.0/(1.0 + x*x);;
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
      StaticScheduler<long long> *sched = getStaticScheduler(nthreads,begin,end,nstep);
      pthread_t threads[nthreads];
      Argument args[nthreads];
      for (unsigned threadId = 0; threadId<nthreads; threadId++) {
         args[threadId].sched = sched;
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
   long long n = 100000L;
   for(int i = 0; i<1000; i++){
      run(n,1);
   }
   run(n,1);
   printf("Count: %d\n",count);
   return 0;
}

