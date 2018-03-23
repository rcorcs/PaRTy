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
class ChunkedScheduler {
private:
   unsigned nthreads;
   T begin;
   T end;
   T step;
   T chunkSize;
   TaskEntry<T> *tasks;
public:
   ChunkedScheduler(unsigned nthreads, T begin, T end, T step, T chunkSize);
   ~ChunkedScheduler() { delete tasks; }
   void updateBounds(T begin, T end, T step, T chunkSize);
   unsigned getNumThreads() { return this->nthreads; }
   void next(unsigned threadId);
   TaskEntry<T> *get(unsigned threadId);
};

template <typename T>
ChunkedScheduler<T>::ChunkedScheduler(unsigned nthreads, T begin, T end, T step, T chunkSize) {
   this->nthreads = nthreads;
   tasks = new TaskEntry<T> [nthreads];
   this->updateBounds(begin,end,step,chunkSize);
}

template <typename T>
void ChunkedScheduler<T>::updateBounds(T begin, T end, T step, T chunkSize) {
   this->begin = begin;
   this->end = end;
   this->step = step;
   this->chunkSize = chunkSize;
   for (unsigned threadId = 0; threadId<nthreads; threadId++) {
      tasks[threadId].begin = begin+threadId*chunkSize*step;
      tasks[threadId].current = tasks[threadId].begin;
      tasks[threadId].end = begin+(threadId+1)*chunkSize*step;
      if (tasks[threadId].end>end) tasks[threadId].end = end;
   }
}

template <typename T>
void ChunkedScheduler<T>::next(unsigned threadId) {
   TaskEntry<T> &entry = tasks[threadId];
   entry.current += step;
}

template <typename T>
TaskEntry<T> *ChunkedScheduler<T>::get(unsigned threadId) {
   TaskEntry<T> *nextPtr = NULL;

   TaskEntry<T> &entry = tasks[threadId];
   if (entry.current<entry.end) nextPtr = &entry;
   else {
      entry.begin += nthreads*chunkSize*step;
      entry.current = entry.begin;
      entry.end = entry.begin + chunkSize*step;
      if (entry.end>end) entry.end = end;
      if (entry.current<end) nextPtr = &entry;
   }

   return nextPtr;
}

template <typename T>
ChunkedScheduler<T> *getChunkedScheduler(unsigned nthreads, T begin, T end, T step, T chunkSize) {
   static ChunkedScheduler<T> *sched = NULL;
   if (sched==NULL) sched = new ChunkedScheduler<T>(nthreads, begin, end, step, chunkSize);
   if (nthreads!=sched->getNumThreads()) {
      delete sched;
      sched = new ChunkedScheduler<T>(nthreads, begin, end, step, chunkSize);
   }else { sched->updateBounds(begin, end, step, chunkSize); }
   return sched;
}

#endif
