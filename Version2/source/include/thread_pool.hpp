#ifndef __THREAD_POOL_HPP__
#define __THREAD_POOL_HPP__

#include <mutex>
#include <vector>
#include <thread>
#include <list>

class Task{
public:
    virtual void run() = 0;
};

class ThreadPool{
public:
    static void WorkerThread(ThreadPool* master);   // 工作线程  每个线程开始时执行这个函数
    ThreadPool(size_t thread_count = 0);
    ~ThreadPool();

    void addTask(Task *task);
    Task *getTask();

    void wait() const;  // 让主线程主动地等待所有的任务执行完成

private:
    bool alive;
    std::vector<std::thread> threads;
    std::list<Task *> tasks;   // 添加和删除操作都是o(1)
    std::mutex lock;        // 用锁来避免添加任务时资源的竞争
};

#endif // __THREAD_POOL_HPP__