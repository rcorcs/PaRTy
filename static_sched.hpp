/***********************************

Classic static partitioning:

Divides the iteration space (nsize)
into num_threads portions of nearly
the same size (nsize/num_threads).

        iteration space        
[------------------------------]

    divided iteration space    
[------][------][------][------]
   t0       t1     t2      t3


The simple static scheduling algorithm divides the number
of loop iterations among the available processors as evenly
as possible, in the hope that each processor receives about
the same amount of work. This algorithm minimizes run-
time synchronization overhead, but does not balance the load
dynamically. If all iterations do not take the same amount
of time, then load imbalance may arise, which will cause
some processors to be idle while other processors continue
to execute loop iterations.

***********************************/

#ifndef EDINBURGH_SCHEDULER_STATIC
#define EDINBURGH_SCHEDULER_STATIC

#include "common.h"

#include <stdlib.h>
#include <math.h>

template <typename T>
class StaticScheduler {
private:
   unsigned nthreads;
   T begin;
   T end;
   T step;
   T range;
   T size;
public:
   StaticScheduler(unsigned nthreads, T begin, T end, T step);
   void updateBounds(T begin, T end, T step);
   TaskEntry<T> get(unsigned threadId);
   unsigned getNumThreads() { return this->nthreads; }
   T getStep() { return this->step; }
};

template <typename T>
StaticScheduler<T>::StaticScheduler(unsigned nthreads, T begin, T end, T step) {
   this->nthreads = nthreads;
   this->updateBounds(begin, end, step);
}

template <typename T>
void StaticScheduler<T>::updateBounds(T begin, T end, T step) {
   this->begin = begin;
   this->end = end;
   this->step = step;

   this->range = ceil(((double)end-begin)/step);
   this->size = (T)ceil(((double)range)/nthreads);
}

template <typename T>
TaskEntry<T> StaticScheduler<T>::get(unsigned threadId) {
   TaskEntry<T> entry;
   entry.begin = begin+(threadId*size)*step;
   entry.begin += (entry.begin-begin)%step;
   entry.current = entry.begin;
   entry.end = begin+((threadId+1)*size)*step;
   entry.end += (entry.end-begin)%step;
   if (entry.end>end) entry.end = end;
   return entry;
}

template <typename T>
StaticScheduler<T> *getStaticScheduler(unsigned nthreads, T begin, T end, T step) {
   static StaticScheduler<T> *sched = NULL;
   if (sched==NULL) sched = new StaticScheduler<T>(nthreads, begin, end, step);
   if (nthreads!=sched->getNumThreads()) {
      delete sched;
      sched = new StaticScheduler<T>(nthreads, begin, end, step);
   }else { sched->updateBounds(begin, end, step); }
   return sched;
}


#endif
