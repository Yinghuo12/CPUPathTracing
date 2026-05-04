#ifndef __THREAD_POOL_HPP__
#define __THREAD_POOL_HPP__

// #include <mutex>  // 采用自己设计的自旋锁
#include "spin_lock.hpp"
#include <functional>
#include <cstddef>
#include <vector>
#include <thread>
#include <queue>

class Task{
public:
    virtual void run() = 0;
    virtual ~Task() = default;
};

class ThreadPool{
public:
    static void WorkerThread(ThreadPool* master);   // 工作线程  每个线程开始时执行这个函数
    ThreadPool(size_t thread_count = 0);
    ~ThreadPool();

    void addTask(Task *task);
    Task *getTask();

    void parallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)> & lambda, bool complex = true); // 并行for循环
    void wait() const;  // 让主线程主动地等待所有的任务执行完成

private:
    std::atomic<int> alive;   // 表示线程池是否还存在  // 之前用bool，换成原子变量
    std::vector<std::thread> threads;
    std::atomic<int> pending_task_count;  // 修改wait的bug
    std::queue<Task *> tasks;   // 添加和删除操作都是o(1)
    // std::mutex lock;        // 用锁来避免添加任务时资源的竞争  被自己的自旋锁替换
    SpinLock spin_lock {};
};

extern ThreadPool thread_pool;
#endif // __THREAD_POOL_HPP__