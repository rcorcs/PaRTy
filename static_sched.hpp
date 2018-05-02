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

#include <stdio.h>


template <typename T>
class StaticContext {
public:
   unsigned nthreads;
   T begin;
   T end;
   T step;
   T range;
   T size;
};

template <typename T>
void CreateStaticContext(StaticContext<T> *ctx, unsigned nthreads, T begin, T end, T step) {
   ctx->nthreads = nthreads;
   ctx->begin = begin;
   ctx->end = end;
   ctx->step = step;

   ctx->range = ceil(((double)end-begin)/step);
   ctx->size = (T)ceil(((double)ctx->range)/nthreads);
}

template <typename T>
TaskEntry<T> ScheduleStaticEntry(const StaticContext<T> *ctx, unsigned threadId) {
   TaskEntry<T> entry;
   entry.begin = ctx->begin+(threadId*ctx->size)*ctx->step;
   entry.begin += (entry.begin-ctx->begin)%ctx->step;

   entry.end = ctx->begin+((threadId+1)*ctx->size)*ctx->step;
   entry.end += (entry.end-ctx->begin)%ctx->step;
   if (entry.end>ctx->end) entry.end = ctx->end;
   return entry;
}

#endif
