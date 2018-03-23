#include <stdio.h>
#include <math.h>


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

int main(){
	long max_num = 5000321L;
	long sum;

		sum = 0;//count the 2 as a prime, then start from 3

#ifdef USE_OMP_STATIC
      #pragma omp parallel for reduction(+:sum) num_threads(8) schedule(static)
#else
      #pragma omp parallel for reduction(+:sum) num_threads(8) schedule(dynamic,64)
#endif
		for(int n = 3; n<max_num; n+=2){ //skip all even numbers
			long count_prime = is_prime(n);
         sum += count_prime;
		}

	printf("maximum number checked: %ld\n", max_num);
	printf("number of primes: %ld\n", sum);

	return 0;
}
