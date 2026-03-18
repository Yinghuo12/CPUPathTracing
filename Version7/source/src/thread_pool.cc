#include "thread_pool.hpp"

// 工作线程
void ThreadPool::WorkerThread(ThreadPool* master){   // 每个线程开始时执行这个函数
    while(master->alive == 1){   // 判断线程池是否存在
        Task *task = master->getTask();  // 不断地获取任务
        if(task != nullptr){
            task->run();
            master->pending_task_count--; // 执行完一个任务，减少待执行任务数量
        } else{
            std::this_thread::yield();   // 优化：把线程的控制权交给操作系统，操作系统就会选择下一个线程执行
        }
    }
}
ThreadPool::ThreadPool(size_t thread_count){
    alive = 1;   // 构造时线程池存在
    pending_task_count = 0; // 初始化待执行任务数量为0
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
    // 这里如果在多线程下，在等待所有任务执行完毕时，该线程被调度出去，其他线程添加任务，之后执行下一句alive=0,工作线程就会停止
    // 如果要改为多线程并行添加任务，要设置两个锁，一个入队的锁，一个出队的锁
    alive = 0; // 析构时线程池销毁
    for (auto &thread : threads) {
        thread.join();
    }
    threads.clear();
}

// 定义并行for循环的任务
class ParallelForTask : public Task {
public:
    ParallelForTask(size_t x, size_t y, const std::function<void(size_t, size_t)> &lambda) 
        : x(x), y(y), lambda(lambda) {}
    void run() override {
        lambda(x, y);
    }
private:  
    size_t x, y;
    std::function<void(size_t, size_t)> lambda;
};

// 批量添加任务：主要功能是 遍历图像的所有像素并行化
void ThreadPool::parallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)> &lambda){ // 并行for循环
    Guard guard(spin_lock);

    for(size_t x = 0; x < width; ++x){
        for(size_t y = 0; y < height; ++y){
            pending_task_count++; // 增加待执行任务数量
            tasks.push(new ParallelForTask(x, y, lambda));
        }
    }
};

// 添加单个任务
void ThreadPool::addTask(Task *task){
    Guard guard(spin_lock); 
    // std::lock_guard<std::mutex> guard(lock); // 获取锁且在函数结束后释放 被自己的自旋锁替换
    pending_task_count++; // 增加待执行任务数量
    tasks.push(task);
}

Task *ThreadPool::getTask(){
    Guard guard(spin_lock);
    // std::lock_guard<std::mutex> guard(lock); // 被自己的自旋锁替换
    if(tasks.empty()){
        return nullptr;
    }
    Task *task = tasks.front();
    tasks.pop();
    return task;
}

 //  让主线程主动地等待所有的任务执行完成
void ThreadPool::wait() const{
    // 这里实现有问题，当最后一个任务在WorkerThread中被获取到，但是还没有执行完毕，主线程就已经销毁了线程池了，所以WorkerThread就没有执行任务，所以这里需要修改一下，等待所有的任务被执行完毕，才销毁线程池
    // 下面是错误的实现
    // while(!tasks.empty()){ // 等待所有的任务被执行完毕，才销毁线程池,否则线程池在master线程还没有获取到任务的时候，线程池就已经析构了，所以并没有执行任务
    while(pending_task_count > 0){ 
        std::this_thread::yield();   // 优化：把线程的控制权交给操作系统，操作系统就会选择下一个线程执行
    }
} 