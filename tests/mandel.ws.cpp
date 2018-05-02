/*
**  PROGRAM: Mandelbrot area
**
**  PURPOSE: Program to compute the area of a  Mandelbrot set.
**           Correct answer should be around 1.510659.
**           WARNING: this program may contain errors
**
**  USAGE:   Program runs without input ... just run the executable
**            
**  HISTORY: Written:  (Mark Bull, August 2011).
**           Changed "complex" to "d_complex" to avoid collsion with 
**           math.h complex type (Tim Mattson, September 2011)
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../sched.h"
#include "utils/execution_timer.h"
unsigned num_threads = 4;


# define NPOINTS 2000
# define MAXITER 1000

int testpoint(struct d_complex c);

struct d_complex{
   double r;
   double i;
};


class Argument {
public:
   WorkStealScheduler<int> *sched;
   unsigned threadId;
   int reduction;
   int PAD[8];
};

void* threadBody(void *arg) {
   Argument *threadArg = ((Argument*)arg);
   WorkStealScheduler<int> *sched = threadArg->sched;
   unsigned threadId = threadArg->threadId;

   while(int *nptr=sched->get(threadId)) {
      int i = *nptr;

      for (int j=0; j<NPOINTS; j++) {
       struct d_complex c;
       c.r = -2.0+2.5*(double)(i)/(double)(NPOINTS)+1.0e-5;
       c.i = 1.125*(double)(j)/(double)(NPOINTS)+1.0e-5;
       threadArg->reduction += testpoint(c);
      }

      sched->next(threadId);
	}

   return NULL;
}


int main(){

   int numoutside = 0;
   double area, error;//, eps  = 1.0e-5;

   __timer_prologue();

//   Loop over grid of points in the complex plane which contains the Mandelbrot set,
//   testing each point to see whether it is inside or outside the set.
/*
   #pragma omp parallel for reduction(+:numoutside)
   for (int i=0; i<NPOINTS; i++) {
     for (int j=0; j<NPOINTS; j++) {
       struct d_complex c;
       c.r = -2.0+2.5*(double)(i)/(double)(NPOINTS)+eps;
       c.i = 1.125*(double)(j)/(double)(NPOINTS)+eps;
       numoutside += testpoint(c);
     }
   }
*/
   {
      unsigned nthreads = num_threads;
      int begin = 0;
      int end = NPOINTS;
      int nstep = 1;
      int reductionIdentity = 0;
      WorkStealScheduler<int> *sched = getWSScheduler(nthreads,begin,end,nstep);
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
         numoutside += args[threadId].reduction;
      }
  }
// Calculate area of set and error estimate and output the results
   
   area=2.0*2.5*1.125*(double)(NPOINTS*NPOINTS-numoutside)/(double)(NPOINTS*NPOINTS);
   error=area/(double)NPOINTS;

   __timer_epilogue();
   printf("Area of Mandlebrot set = %12.8f +/- %12.8f\n",area,error);

}

int testpoint(struct d_complex c){

// Does the iteration z=z*z+c, until |z| > 2 when point is known to be outside set
// If loop count reaches MAXITER, point is considered to be inside the set

       struct d_complex z;
       int iter;
       double temp;

       z=c;
       for (iter=0; iter<MAXITER; iter++){
         temp = (z.r*z.r)-(z.i*z.i)+c.r;
         z.i = z.r*z.i*2+c.i;
         z.r = temp;
         if ((z.r*z.r+z.i*z.i)>4.0) {
           return 1;
         }
       }

      return 0;
}

