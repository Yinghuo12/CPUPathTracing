#ifndef __SPIN_LOCK_HPP__
#define __SPIN_LOCK_HPP__

#include <atomic>
#include <thread>

// 自旋锁类
class SpinLock {
public:
    void acquire() { while (flag.test_and_set(std::memory_order_acquire)) { std::this_thread::yield(); } }
    void release() { flag.clear(std::memory_order_release); }
private:
    std::atomic_flag flag {};
};

// 构造函数获取到这个锁，析构的时候释放这个锁
class Guard {
public:
    Guard(SpinLock &spin_lock) : spin_lock(spin_lock) { spin_lock.acquire(); }
    ~Guard() { spin_lock.release(); }
private:
    SpinLock &spin_lock;
};

#endif