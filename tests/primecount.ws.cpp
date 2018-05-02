#include <stdio.h>
#include<math.h>

#include "../sched.h"
#include "utils/execution_timer.h"
unsigned num_threads = 4;

int is_prime(long num){
	if(num<=1) return 0;
	else if(num>3){
		if(num%2==0) return 0;
		long max_divisor = sqrt(num);
		for(long d = 3; d<=max_divisor; d+=2){
			if(num%d==0) return 0;
		}
	}
	return 1;
}

class Argument {
public:
   WorkStealScheduler<long> *sched;
   unsigned threadId;
   long reduction;
};

void* threadBody(void *arg) {
   Argument *threadArg = ((Argument*)arg);
   WorkStealScheduler<long> *sched = threadArg->sched;
   unsigned threadId = threadArg->threadId;
   while(long *nptr=sched->get(threadId)) {
      long n = *nptr;

		long count_prime = is_prime(n);
      threadArg->reduction += count_prime;

      sched->next(threadId);
	}
   return NULL;
}

int main(){
	long max_num = 5000321L;
	long sum;

   __timer_prologue();

		sum = 0;//count the 2 as a prime, then start from 3
      
		//for(int n = 0; n<max_num; n++){ //skip all even numbers
      unsigned nthreads = num_threads;
      long begin = 3;
      long end = max_num;
      long step = 2;
      long reductionIdentity = 0;
      WorkStealScheduler<long> *sched = getWSScheduler(nthreads,begin,end,step);
      pthread_t threads[nthreads];
      Argument args[nthreads];
      for (unsigned threadId = 0; threadId<nthreads; threadId++) {
         args[threadId].sched = sched;
         args[threadId].threadId = threadId;
         args[threadId].reduction = reductionIdentity;
         pthread_create(&threads[threadId],NULL,threadBody,static_cast<void*>(&args[threadId]));
      }
      for (unsigned threadId = 0; threadId<nthreads; threadId++) {
         pthread_join(threads[threadId],NULL);
         sum += args[threadId].reduction;
      }
   __timer_epilogue();
	printf("maximum number checked: %ld\n", max_num);
	printf("number of primes: %ld\n", sum);

	return 0;
}
