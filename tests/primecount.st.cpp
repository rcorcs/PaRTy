#include <stdio.h>
#include<math.h>

#include "../sched.h"
#include "../execution_timer.h"
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
   StaticContext<long> *ctx;
   unsigned threadId;
   long reduction;
};

CREATE_STATIC_THREAD(threadBody, Argument, 2, long, n,
  {}
, 
  long count_prime = is_prime(n);
  ThreadArgs->reduction += count_prime;
)


int main(){
	long max_num = 5000321L;
	long sum;

   __timer_prologue();

		sum = 0;//count the 2 as a prime, then start from 3
      
      LAUNCH_STATIC_THREADS(num_threads, long, 3, max_num, 2, Argument, threadBody, 
         args[threadId].reduction = 0;
      ,
         sum += args[threadId].reduction;
      )

   __timer_epilogue();
	printf("maximum number checked: %ld\n", max_num);
	printf("number of primes: %ld\n", sum);

	return 0;
}
