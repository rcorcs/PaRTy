
#ifndef EDINBURGH_SCHEDULER_WORKSTEAL
#define EDINBURGH_SCHEDULER_WORKSTEAL

#include "common.h"

#include <stdlib.h>
#include <math.h>

#include <pthread.h>

#include <atomic>

template <typename T>
class ChunkedWSContext {
public:
   unsigned nthreads;
   T begin;
   T end;
   T step;
   T range;
   unsigned chunkSize;
   T *startPoint;
   int *numChunks;
   pthread_mutex_t *mutex;
};

template <typename T>
void CreateChunkedWSContext(ChunkedWSContext<T> *ctx, unsigned nthreads, T begin, T end, T step, unsigned chunkSize) {
   ctx->nthreads = nthreads;
   ctx->begin = begin;
   ctx->end = end;
   ctx->step = step;
   ctx->chunkSize = chunkSize;

   ctx->range = _ceil_div(end-begin,step);
   T accumChunks = 0;
   ctx->numChunks = new int[nthreads];
   ctx->startPoint = new T[nthreads];
   ctx->mutex = new pthread_mutex_t[nthreads];
   for (unsigned threadId = 0; threadId<nthreads; threadId++) {
      ctx->numChunks[threadId] = _ceil_div(ctx->range, chunkSize*nthreads);
      ctx->startPoint[threadId] = ctx->begin + accumChunks*ctx->chunkSize*ctx->step;
      accumChunks += ctx->numChunks[threadId];
      pthread_mutex_init(&(ctx->mutex[threadId]), NULL);
   }

}

template <typename T>
bool NextChunkedWSStart(ChunkedWSContext<T> *ctx, unsigned threadId) {
   unsigned neighbourId;
   neighbourId = (threadId+1)%ctx->nthreads;
   while (neighbourId!=threadId) {
      if (ctx->numChunks[neighbourId]>1) {
         pthread_mutex_lock(&(ctx->mutex[neighbourId]));
           T nChunks = ctx->numChunks[neighbourId];
           T newChunks = nChunks/2;
           ctx->numChunks[neighbourId] -= newChunks;
           T start = ctx->startPoint[neighbourId] + (nChunks-newChunks)*ctx->chunkSize*ctx->step;
         pthread_mutex_unlock(&(ctx->mutex[neighbourId]));

         pthread_mutex_lock(&(ctx->mutex[threadId]));
           ctx->numChunks[threadId] = newChunks;
           ctx->startPoint[threadId] = start;
         pthread_mutex_unlock(&(ctx->mutex[threadId]));
         return true;
      }
      neighbourId = (neighbourId+1)%ctx->nthreads;
   }
   return false;
}

#define CREATE_CHUNKED_WS_THREAD(_NAME_, _ARG_TYPE_, _ITER_STEP_, _CHUNK_SIZE_, _ITER_TYPE_, _ITER_NAME_, _VAR_DEFS_, _BODY_) \
void* _NAME_(void *arg) { \
   _ARG_TYPE_ *ThreadArgs = ((_ARG_TYPE_*)arg); \
   ChunkedWSContext<_ITER_TYPE_> *_sched_ctx = ThreadArgs->ctx; \
   unsigned _sched_threadId = ThreadArgs->threadId; \
   _VAR_DEFS_ \
   const unsigned _sched_chunk_size = _CHUNK_SIZE_; \
   const _ITER_TYPE_ _sched_iter_step = _ITER_STEP_; \
   const _ITER_TYPE_ _sched_step_factor = _sched_chunk_size*_sched_iter_step; \
   do {\
     while(_sched_ctx->numChunks[_sched_threadId]>0) { \
        _ITER_TYPE_ _sched_start = _sched_ctx->startPoint[_sched_threadId]; \
        _ITER_TYPE_ end = _sched_start + _sched_chunk_size*_sched_iter_step; \
        end = (end>_sched_ctx->end)?_sched_ctx->end:end; \
        for (_ITER_TYPE_ _sched_iter_id = _sched_start; _sched_iter_id<end; _sched_iter_id += _sched_iter_step) { \
            _ITER_TYPE_ _ITER_NAME_ = _sched_iter_id; \
            { _BODY_ } \
        } \
        pthread_mutex_lock(&(_sched_ctx->mutex[_sched_threadId])); \
        _sched_ctx->startPoint[_sched_threadId] += _sched_step_factor; \
        _sched_ctx->numChunks[_sched_threadId]--; \
        pthread_mutex_unlock(&(_sched_ctx->mutex[_sched_threadId])); \
     } \
   } while(NextChunkedWSStart(_sched_ctx,_sched_threadId)); \
   return NULL; \
}

#define LAUNCH_CHUNKED_WS_THREADS(_NUM_THREADS_, _TYPE_, _BEGIN_, _END_, _STEP_, _CHUNK_SIZE_, _ARG_TYPE_, _THREAD_NAME_, _ARGS_SETUP_, _REDUCTIONS_) \
{ \
      unsigned nthreads = _NUM_THREADS_; \
      _TYPE_ begin = _BEGIN_; \
      _TYPE_ end = _END_; \
      _TYPE_ nstep = _STEP_; \
      unsigned chunkSize = _CHUNK_SIZE_; \
      ChunkedWSContext<_TYPE_> ctx; \
      CreateChunkedWSContext(&ctx, nthreads,begin,end,nstep,chunkSize); \
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
      delete ctx.numChunks; \
      delete ctx.startPoint; \
      delete ctx.mutex; \
}


#endif
