#ifndef __SPIN_LOCK_HPP__
#define __SPIN_LOCK_HPP__

#include <atomic>
#include <thread>

// 自旋锁类
class SpinLock {
public:
    void acquire() { while (flag.test_and_set(std::memory_order_acquire)) { std::this_thread::yield(); } }  // 通过test_and_set来测试和设置这个flag，如果已经被设置了，就让出CPU，继续尝试获取锁
    void release() { flag.clear(std::memory_order_release); }   // 释放锁就是把这个flag清除掉
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