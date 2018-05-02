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
   ChunkedContext<long> *ctx;
   unsigned threadId;
   long reduction;
};

void* threadBody(void *arg) {
   Argument *threadArg = ((Argument*)arg);
   ChunkedContext<long> *ctx = threadArg->ctx;
   unsigned threadId = threadArg->threadId;

   //BEGIN SHARED VARIABLES HERE
   //END SHARED VARIABLES HERE

  //UPDATE SETTINGS
   const unsigned chunk_size = 16;
   const long iter_step = 2; //ctx->step=iter_step

   long start = ScheduleChunkedStartPoint(ctx,threadId);

   for (unsigned chunk_id = 0; chunk_id<ctx->numChunks-1; chunk_id++) {
     for (unsigned iter_id = 0; iter_id < chunk_size; iter_id++) {
          long iter_idx = start + iter_id*iter_step; 

  //COPY ITERATOR HERE
          long n = iter_idx;
  //BEGIN KERNEL HERE
        long count_prime = is_prime(n);
        threadArg->reduction += count_prime;
  //END KERNEL HERE

      }
      start += ctx->nthreads*chunk_size*iter_step;
   }

   long end = ScheduleChunkedEndPoint(ctx,threadId,start);
   for (long iter_idx = start; iter_idx<end; iter_idx += iter_step) {

  //COPY ITERATOR HERE
       long n = iter_idx;
  //BEGIN KERNEL HERE
        long count_prime = is_prime(n);
        threadArg->reduction += count_prime;
  //END KERNEL HERE
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
      long nstep = 2;
      unsigned chunkSize = 16;
      long reductionIdentity = 0;
      //ChunkedScheduler<long> *sched = getChunkedScheduler(nthreads,begin,end,step,chunkSize);
      ChunkedContext<long> ctx;
      CreateChunkedContext(&ctx, nthreads,begin,end,nstep,chunkSize);
      pthread_t threads[nthreads];
      Argument args[nthreads];
      for (unsigned threadId = 0; threadId<nthreads; threadId++) {
         args[threadId].ctx = &ctx;
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
