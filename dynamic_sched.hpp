/***********************************

Dynamic scheduling.

* If chunk-size = 1, then it represents the
classic self-scheduling.

The simplest dynamic algorithm for scheduling loop iter-
ations is called self-scheduling [25], [28]. In this algorithm,
each processor repeatedly executes one iteration of the loop
until all iterations are executed. The algorithm relies on a
central work queue of iterations, where each idle processor
gets one iteration, executes it, and repeats the same cycle
until there are no more iterations to execute. Self-scheduling
achieves almost perfect load balancing, since all processors
finish within one iteration of each other. Unfortunately, this
algorithm incurs significant synchronization overhead; each
iteration requires atomic access to the central work queue. This
synchronization overhead can quickly become a bottleneck in
large-scale systems, or even in small-scale systems if the time
to execute one iteration is small.

        iteration space        
[------------------------------]

    divided iteration space    
[-][-][-][-][-][-][-][-][-][-][-][-]
t0 t1 t2 t3 t0 t1 t2 t3 t0 t1 t2 t3 


[25] B. Smith. "Architecture and applications of the HEP computer system",
Proc. SPIE, Real-Time Signal Processing IV, 1981

[28] P. Tang and P.-C. Yew, "Processor self-scheduling for multiple nested
parallel loops", Proc. I986 Int. Con$ Parallel Processing, 1986, pp.
5 28-535.


* In the general case of the chunk-size,
it represents the Uniform-sized chunking
technique.

Uniform-sized chunking [16] reduces synchronization over-
head by having each processor take K iterations, instead of
one. This algorithm amortizes the cost of each synchronization
operation over the execution time of K iterations, resulting in
less synchronization overhead. Uniform-sized chunking has a
greater potential for imbalance than self-scheduling however,
as processors finish within K iterations of each other in the
worst case. In addition, choosing an appropriate value for K
is a difficult problem, which has been solved for limited cases
only.

[16] C. P. Kruskal and A. Weiss, "Allocating independent subtasks on parallel
processors", IEEE Trans. Sojhare Eng., vol. 11, no. IO, pp. 1001-1016,
Oct. 1985.

***********************************/

#ifndef EDINBURGH_SCHEDULER_DYNAMIC
#define EDINBURGH_SCHEDULER_DYNAMIC

#include "common.h"

#include <stdlib.h>
#include <math.h>

#include <pthread.h>

#include <atomic>


template <typename T>
class DynamicContext {
public:
   unsigned nthreads;
   T begin;
   T end;
   T step;
   std::atomic<T> nextChunkStart;
   unsigned chunkSize;
   unsigned numChunks;
};

template <typename T>
void CreateDynamicContext(DynamicContext<T> *ctx, unsigned nthreads, T begin, T end, T step, unsigned chunkSize) {
   ctx->nthreads = nthreads;
   ctx->begin = begin;
   ctx->end = end;
   ctx->step = step;
   ctx->chunkSize = chunkSize;

   ctx->nextChunkStart = begin;

   T range = (T)ceil(((double)end-begin)/step);
   ctx->numChunks = (unsigned)ceil( ((double)range)/((double)chunkSize*nthreads) );
}

template <typename T>
T ScheduleNextDynamicStartPoint(DynamicContext<T> *ctx) {
  return std::atomic_fetch_add(&ctx->nextChunkStart, (T)ctx->chunkSize*ctx->step);
}

#endif
