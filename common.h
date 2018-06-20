
#ifndef EDINBURGH_SCHEDULER_COMMON
#define EDINBURGH_SCHEDULER_COMMON

template <typename T>
class TaskEntry {
public:
   T begin;
   T end;
};

#define _ceil_div(_X_, _Y_) ((_X_)/(_Y_)+((_X_)%(_Y_) > 0))

#endif
