/***********************************

Chunked-static partitioning:

        iteration space        
[------------------------------]

    divided iteration space    
[-][-][-][-][-][-][-][-][-][-][-][-]
t0 t1 t2 t3 t0 t1 t2 t3 t0 t1 t2 t3 


***********************************/

#ifndef EDINBURGH_SCHEDULER_CHUNKED
#define EDINBURGH_SCHEDULER_CHUNKED

#include "common.h"

#include <stdlib.h>
#include <math.h>

template <typename T>
class ChunkedContext {
public:
   unsigned nthreads;
   T begin;
   T end;
   T step;
   T range;
   unsigned chunkSize;
   unsigned numChunks;
};

template <typename T>
void CreateChunkedContext(ChunkedContext<T> *ctx, unsigned nthreads, T begin, T end, T step, unsigned chunkSize) {
   ctx->nthreads = nthreads;
   ctx->begin = begin;
   ctx->end = end;
   ctx->step = step;
   ctx->chunkSize = chunkSize;

   ctx->range = (T)ceil(((double)end-begin)/step);
   ctx->numChunks = (unsigned)ceil( ((double)ctx->range)/((double)chunkSize*nthreads) );
}

template <typename T>
T ScheduleChunkedStartPoint(const ChunkedContext<T> *ctx, unsigned threadId) {
   T start = ctx->begin+threadId*ctx->chunkSize*ctx->step;
   return start;
}


template <typename T>
T ScheduleChunkedEndPoint(const ChunkedContext<T> *ctx, unsigned threadId, T start) {
   T end = start + ctx->chunkSize*ctx->step;
   end = (end>ctx->end)?ctx->end:end;
   return end;
}

#endif
