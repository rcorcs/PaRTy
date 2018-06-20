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
#include "../execution_timer.h"
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
   StaticContext<int> *ctx;
   unsigned threadId;
   int reduction;
};

CREATE_STATIC_THREAD(threadBody, Argument, 1, int, i,
  {}
, 
  for (int j=0; j<NPOINTS; j++) {
    struct d_complex c;
    c.r = -2.0+2.5*(double)(i)/(double)(NPOINTS)+1.0e-5;
    c.i = 1.125*(double)(j)/(double)(NPOINTS)+1.0e-5;
    ThreadArgs->reduction += testpoint(c);
  }
)

int main(){

   int numoutside = 0;
   double area, error;//, eps  = 1.0e-5;

   __timer_prologue();

//   Loop over grid of points in the complex plane which contains the Mandelbrot set,
//   testing each point to see whether it is inside or outside the set.

   LAUNCH_STATIC_THREADS(num_threads, int, 0, NPOINTS, 1, Argument, threadBody, 
         args[threadId].reduction = 0;
   ,
         numoutside += args[threadId].reduction;
   )

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

