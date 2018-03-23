
#ifndef EDINBURGH_SCHEDULER_COMMON
#define EDINBURGH_SCHEDULER_COMMON

template <typename T>
class TaskEntry {
public:
   T begin;
   T end;
   T current;
//   int PAD[8];
};

#endif
