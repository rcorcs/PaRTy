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

   ctx->range = _ceil_div(end-begin,step);
   ctx->numChunks = _ceil_div(ctx->range,chunkSize*nthreads);
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

#define CREATE_CHUNKED_THREAD(_NAME_, _ARG_TYPE_, _ITER_STEP_, _CHUNK_SIZE_, _ITER_TYPE_, _ITER_NAME_, _VAR_DEFS_, _BODY_) \
void* _NAME_(void *arg) { \
   _ARG_TYPE_ *ThreadArgs = ((_ARG_TYPE_*)arg); \
   ChunkedContext<_ITER_TYPE_> *_sched_ctx = ThreadArgs->ctx; \
   unsigned _sched_threadId = ThreadArgs->threadId; \
   _VAR_DEFS_ \
   const unsigned _sched_chunk_size = _CHUNK_SIZE_; \
   const _ITER_TYPE_ _sched_iter_step = _ITER_STEP_; \
   const _ITER_TYPE_ _sched_step_factor = _sched_ctx->nthreads*_sched_chunk_size*_sched_iter_step; \
   _ITER_TYPE_ _sched_start = _sched_ctx->begin + _sched_threadId*_sched_chunk_size*_sched_iter_step; \
   for (unsigned _sched_chunk_id = 0; _sched_chunk_id<_sched_ctx->numChunks-1; _sched_chunk_id++) { \
      for (unsigned _sched_iter_id = 0; _sched_iter_id < _sched_chunk_size; _sched_iter_id++) { \
          _ITER_TYPE_ _ITER_NAME_ = _sched_start + (_ITER_TYPE_)(_sched_iter_id*_sched_iter_step); \
          { _BODY_ } \
      } \
      _sched_start += _sched_step_factor; \
   } \
   _ITER_TYPE_ end = _sched_start + _sched_chunk_size*_sched_iter_step; \
   end = (end>_sched_ctx->end)?_sched_ctx->end:end; \
   for (_ITER_TYPE_ _sched_iter_id = _sched_start; _sched_iter_id<end; _sched_iter_id += _sched_iter_step) { \
      _ITER_TYPE_ _ITER_NAME_ = _sched_iter_id; \
      { _BODY_ } \
   } \
   return NULL; \
}

#define LAUNCH_CHUNKED_THREADS(_NUM_THREADS_, _TYPE_, _BEGIN_, _END_, _STEP_, _CHUNK_SIZE_, _ARG_TYPE_, _THREAD_NAME_, _ARGS_SETUP_, _REDUCTIONS_) \
{ \
      unsigned nthreads = _NUM_THREADS_; \
      _TYPE_ begin = _BEGIN_; \
      _TYPE_ end = _END_; \
      _TYPE_ nstep = _STEP_; \
      unsigned chunkSize = _CHUNK_SIZE_; \
      ChunkedContext<_TYPE_> ctx; \
      CreateChunkedContext(&ctx, nthreads,begin,end,nstep,chunkSize); \
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
