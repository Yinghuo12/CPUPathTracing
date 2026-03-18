#include "thread_pool.hpp"

// 工作线程
void ThreadPool::WorkerThread(ThreadPool* master){   // 每个线程开始时执行这个函数
    while(master->alive == true){   // 判断线程池是否存在
        Task *task = master->getTask();  // 不断地获取任务
        if(task != nullptr){
            task->run();
        } else{
            std::this_thread::yield();   // 优化：把线程的控制权交给操作系统，操作系统就会选择下一个线程执行
        }
    }
}
ThreadPool::ThreadPool(size_t thread_count){
    alive = true;   // 构造时线程池存在
    if (thread_count == 0) {
        thread_count = std::thread::hardware_concurrency();  // 返回用户计算机上可用于运行线程的逻辑处理器数量
    }
    for(size_t i = 0; i < thread_count; ++i){
        threads.push_back(std::thread(ThreadPool::WorkerThread, this));
    }
}

ThreadPool::~ThreadPool(){
    wait();
    // 这里仅假设在主线程中添加任务
    // 这里如果在多线程下，在等待所有任务执行完毕时，该线程被调度出去，其他线程添加任务，之后执行下一句alive=false,工作线程就会停止
    // 如果要改为多线程并行添加任务，要设置两个锁，一个入队的锁，一个出队的锁

    // 等待任务队列执行完毕
    wait();
    alive = false; // 析构时线程池销毁
    for (auto &thread : threads) {
        thread.join();
    }
    threads.clear();
}

void ThreadPool::addTask(Task *task){
    std::lock_guard<std::mutex> guard(lock); // 获取锁且在函数结束后释放 
    tasks.push_back(task);
}

Task *ThreadPool::getTask(){
    std::lock_guard<std::mutex> guard(lock); 
    if(tasks.empty()){
        return nullptr;
    }
    Task *task = tasks.front();
    tasks.pop_front();
    return task;
}



 //  让主线程主动地等待所有的任务执行完成
void ThreadPool::wait() const{
    while(!tasks.empty()){ // 等待所有的任务被执行完毕，才销毁线程池,否则线程池在master线程还没有获取到任务的时候，线程池就已经析构了，所以并没有执行任务
        std::this_thread::yield();   // 优化：把线程的控制权交给操作系统，操作系统就会选择下一个线程执行
    }
} 