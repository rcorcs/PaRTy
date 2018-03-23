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

template <typename T>
class DynamicScheduler {
private:
   unsigned nthreads;
   T begin;
   T end;
   T step;
   T chunkSize;
   TaskEntry<T> *tasks;
   TaskEntry<T> nextEntry;
   pthread_mutex_t mutex;
public:
   DynamicScheduler(unsigned nthreads, T begin, T end, T step, T chunkSize);
   ~DynamicScheduler() { delete tasks; }
   void updateBounds(T begin, T end, T step, T chunkSize);
   unsigned getNumThreads() { return this->nthreads; }
   void next(unsigned threadId);
   TaskEntry<T> *get(unsigned threadId);
};

template <typename T>
DynamicScheduler<T>::DynamicScheduler(unsigned nthreads, T begin, T end, T step, T chunkSize) {
   this->nthreads = nthreads;
   tasks = new TaskEntry<T> [nthreads];
   this->updateBounds(begin,end,step,chunkSize);

   pthread_mutex_init(&mutex, NULL);
}

template <typename T>
void DynamicScheduler<T>::updateBounds(T begin, T end, T step, T chunkSize) {
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

   nextEntry.begin = begin+nthreads*chunkSize*step;
   nextEntry.current = nextEntry.begin;
   nextEntry.end = begin+(nthreads+1)*chunkSize*step;
   if (nextEntry.end>end) nextEntry.end = end;
}

template <typename T>
void DynamicScheduler<T>::next(unsigned threadId) {
   TaskEntry<T> &entry = tasks[threadId];
   entry.current += step;
}

template <typename T>
TaskEntry<T> *DynamicScheduler<T>::get(unsigned threadId) {
   TaskEntry<T> *nextPtr = NULL;

   TaskEntry<T> &entry = tasks[threadId];
   if (entry.current<entry.end) nextPtr = &entry;
   else {

      pthread_mutex_lock(&mutex);
      nextEntry.begin = nextEntry.end;
      nextEntry.current = nextEntry.begin;
      nextEntry.end += chunkSize*step;
      if (nextEntry.end>end) nextEntry.end = end;
      tasks[threadId] = nextEntry;
      pthread_mutex_unlock(&mutex);

      if (tasks[threadId].current<end) nextPtr = &tasks[threadId];
   }

   return nextPtr;
}

template <typename T>
DynamicScheduler<T> *getDynamicScheduler(unsigned nthreads, T begin, T end, T step, T chunkSize) {
   static DynamicScheduler<T> *sched = NULL;
   if (sched==NULL) sched = new DynamicScheduler<T>(nthreads, begin, end, step, chunkSize);
   if (nthreads!=sched->getNumThreads()) {
      delete sched;
      sched = new DynamicScheduler<T>(nthreads, begin, end, step, chunkSize);
   }else { sched->updateBounds(begin, end, step, chunkSize); }
   return sched;
}

#endif
