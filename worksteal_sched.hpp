
#ifndef EDINBURGH_SCHEDULER_WORKSTEAL
#define EDINBURGH_SCHEDULER_WORKSTEAL

#include "common.h"

#include <stdlib.h>
#include <math.h>

#include <pthread.h>

#include <atomic>


template <typename T>
class WorkStealScheduler {
private:
   unsigned nthreads;
   T begin;
   T end;
   T step;
   TaskEntry<T> *tasks;
   pthread_mutex_t *mutex;
   std::atomic<int> needsLock;
public:
   WorkStealScheduler(unsigned nthreads, T begin, T end, T step);
   ~WorkStealScheduler() { delete tasks; delete mutex;}
   void updateBounds(T begin, T end, T step);
   unsigned getNumThreads() { return this->nthreads; }
   void next(unsigned threadId);
   T *get(unsigned threadId);
};

template <typename T>
WorkStealScheduler<T>::WorkStealScheduler(unsigned nthreads, T begin, T end, T step) {
   this->nthreads = nthreads;
   this->begin = begin;
   this->end = end;
   this->step = step;
   needsLock = 0;
   tasks = new TaskEntry<T> [nthreads];
   mutex = new pthread_mutex_t[nthreads];

   T range = ceil(((double)end-begin)/step);
   T size = (T)ceil(((double)range)/nthreads);
   for (unsigned threadId = 0; threadId<nthreads; threadId++) {
      tasks[threadId].begin = begin+(threadId*size)*step;
      tasks[threadId].begin += (tasks[threadId].begin-begin)%step;

      tasks[threadId].end = begin+((threadId+1)*size)*step;
      tasks[threadId].end += (tasks[threadId].end-begin)%step;
      if (tasks[threadId].end>end) tasks[threadId].end = end;
      pthread_mutex_init(&(mutex[threadId]), NULL);
   }
}

template <typename T>
void WorkStealScheduler<T>::updateBounds(T begin, T end, T step) {
   this->begin = begin;
   this->end = end;
   this->step = step;
   needsLock = 0;
   T range = ceil(((double)end-begin)/step);
   T size = (T)ceil(((double)range)/nthreads);
   for (unsigned threadId = 0; threadId<nthreads; threadId++) {
      tasks[threadId].begin = begin+(threadId*size)*step;
      tasks[threadId].begin += (tasks[threadId].begin-begin)%step;
      tasks[threadId].end = begin+((threadId+1)*size)*step;
      tasks[threadId].end += (tasks[threadId].end-begin)%step;
      if (tasks[threadId].end>end) tasks[threadId].end = end;
   }
}

template <typename T>
void WorkStealScheduler<T>::next(unsigned threadId) {
   TaskEntry<T> &entry = tasks[threadId];
   entry.begin += step;
}

template <typename T>
T *WorkStealScheduler<T>::get(unsigned threadId) {
   T *nextPtr = NULL;

   if (needsLock){
      pthread_mutex_lock(&(mutex[threadId]));
      TaskEntry<T> &entry = tasks[threadId];
      if (entry.begin<entry.end) nextPtr = &entry.begin;
      pthread_mutex_unlock(&(mutex[threadId]));
   } else {
      TaskEntry<T> &entry = tasks[threadId];
      if (entry.begin<entry.end) nextPtr = &entry.begin;
   }

   if (nextPtr==NULL) {
      needsLock = 1;
      unsigned neighbourId;
      neighbourId = (threadId+rand()%(nthreads-1))%nthreads;
      if (neighbourId==threadId || (tasks[neighbourId].begin + 8*step) >= tasks[neighbourId].end) {
         neighbourId = (threadId+1)%nthreads;
         //neighbourId = threadId?(threadId-1):(nthreads-1);

         while (neighbourId!=threadId && (tasks[neighbourId].begin + 8*step) >= tasks[neighbourId].end) {
            neighbourId = (neighbourId+1)%nthreads;
            //neighbourId = neighbourId?(neighbourId-1):(nthreads-1);
         }
      }
      if (neighbourId!=threadId) {
         
         pthread_mutex_lock(&(mutex[neighbourId]));

         TaskEntry<T> &neighbourEntry = tasks[neighbourId];

         T thisBegin = neighbourEntry.begin;
         T thisEnd = neighbourEntry.end;
         T range = ceil(((double)thisEnd-thisBegin)/step);
         //if (range>0) {
            //T size = (T)ceil(((double)range)/2);
            //T size = (T)ceil(((double)range)/3);
            T size = (T)ceil(((double)range)/2);

            neighbourEntry.end = thisBegin + (size)*step;
            neighbourEntry.end += (neighbourEntry.end-thisBegin)%step;
            if (neighbourEntry.end>thisEnd) neighbourEntry.end = thisEnd;
         pthread_mutex_lock(&(mutex[threadId]));
         pthread_mutex_unlock(&(mutex[neighbourId]));
            TaskEntry<T> &entry = tasks[threadId];
            entry.begin = thisBegin + (size)*step;
            entry.begin += (entry.begin-thisBegin)%step;
            entry.end = thisEnd;
            if (entry.begin<entry.end) nextPtr = &entry.begin;
         pthread_mutex_unlock(&(mutex[threadId]));

      }
   }

   return nextPtr;
}

template <typename T>
WorkStealScheduler<T> *getWSScheduler(unsigned nthreads, T begin, T end, T step) {
   static WorkStealScheduler<T> *sched = NULL;
   if (sched==NULL) sched = new WorkStealScheduler<T>(nthreads, begin, end, step);
   if (nthreads!=sched->getNumThreads()) {
      delete sched;
      sched = new WorkStealScheduler<T>(nthreads, begin, end, step);
   }else { sched->updateBounds(begin, end, step); }
   return sched;
}

#endif
