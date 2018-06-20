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

   ctx->range = _ceil_div(end-begin,step);
   ctx->size = _ceil_div(ctx->range,nthreads);
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

#define CREATE_STATIC_THREAD(_NAME_, _ARG_TYPE_, _ITER_STEP_, _ITER_TYPE_, _ITER_NAME_, _VAR_DEFS_, _BODY_) \
void* _NAME_(void *arg) { \
   _ARG_TYPE_ *ThreadArgs = ((_ARG_TYPE_*)arg); \
   StaticContext<_ITER_TYPE_> *_sched_ctx = ThreadArgs->ctx; \
   unsigned _sched_threadId = ThreadArgs->threadId; \
   _VAR_DEFS_ \
   const _ITER_TYPE_ _sched_iter_step = _ITER_STEP_; \
   TaskEntry<_ITER_TYPE_> _sched_entry = ScheduleStaticEntry(_sched_ctx,_sched_threadId); \
   for (_ITER_TYPE_ _sched_iter_idx = _sched_entry.begin; _sched_iter_idx<_sched_entry.end; _sched_iter_idx += _sched_iter_step) { \
          _ITER_TYPE_ _ITER_NAME_ = _sched_iter_idx; \
          { _BODY_ } \
	} \
   return NULL; \
}

#define LAUNCH_STATIC_THREADS(_NUM_THREADS_, _TYPE_, _BEGIN_, _END_, _STEP_, _ARG_TYPE_, _THREAD_NAME_, _ARGS_SETUP_, _REDUCTIONS_) \
{ \
      unsigned nthreads = _NUM_THREADS_; \
      _TYPE_ begin = _BEGIN_; \
      _TYPE_ end = _END_; \
      _TYPE_ nstep = _STEP_; \
      StaticContext<_TYPE_> ctx; \
      CreateStaticContext(&ctx, nthreads,begin,end,nstep); \
      pthread_t threads[nthreads]; \
      _ARG_TYPE_ args[nthreads]; \
      for (unsigned threadId = 0; threadId<nthreads; threadId++) { \
         args[threadId].ctx = &ctx; \
         args[threadId].threadId = threadId; \
         _ARGS_SETUP_ \
         pthread_create(&threads[threadId],NULL,_THREAD_NAME_,static_cast<void*>(&args[threadId])); \
      } \
      for (unsigned threadId = 0; threadId<nthreads; threadId++) { \
         pthread_join(threads[threadId],NULL); \
         _REDUCTIONS_ \
      } \
}

#endif
