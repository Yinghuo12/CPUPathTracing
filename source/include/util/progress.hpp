#ifndef __PROGRESS_HPP__
#define __PROGRESS_HPP__

#include "thread/thread_pool.hpp"

class Progress {
public:
    Progress(size_t total, size_t step = 1);

    void update(size_t count);
private:
    size_t total, current;
    size_t percent, last_percent, step;
    SpinLock spin_lock;
};

#endif // __PROGRESS_HPP__