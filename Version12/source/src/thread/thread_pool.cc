#include "thread/thread_pool.hpp"
#include <cmath>

ThreadPool thread_pool{};  // 全局线程池  需要在头文件extern出来

// 工作线程
void ThreadPool::WorkerThread(ThreadPool* master){   // 每个线程开始时执行这个函数
    while(master->alive == 1){   // 判断线程池是否存在
        // Lesson10_6_buckets_bvh_optimization
        if(master->tasks.empty()){
            std::this_thread::sleep_for(std::chrono::milliseconds(2)); // 优化：线程休眠2ms
            continue;
        }
        Task *task = master->getTask();  // 不断地获取任务
        if(task != nullptr){
            task->run();
            delete task;
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
    ParallelForTask(size_t x, size_t y, size_t chunk_width, size_t chunk_height, const std::function<void(size_t, size_t)> &lambda) 
        : x(x), y(y), chunk_width(chunk_width), chunk_height(chunk_height), lambda(lambda) {}
    void run() override {
        for(size_t idx_x = 0; idx_x < chunk_width; idx_x++){
            for(size_t idx_y = 0; idx_y < chunk_height; idx_y++){
                lambda(x + idx_x, y + idx_y);
            }
        }
    }
private:  
    size_t x, y, chunk_width, chunk_height;
    std::function<void(size_t, size_t)> lambda;
};

// 批量添加任务：主要功能是 遍历图像的所有像素并行化
void ThreadPool::parallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)> &lambda, bool complex){ // 并行for循环
    Guard guard(spin_lock);

    float chunk_width_float = static_cast<float>(width) / std::sqrt(threads.size());  // 每个线程处理的像素块大小
    float chunk_height_float = static_cast<float>(height) / std::sqrt(threads.size()); // 每个线程处理的像素块大小
    if (complex){
        chunk_width_float /= std::sqrt(16);  // 每个线程处理的像素块大小
        chunk_height_float /= std::sqrt(16); // 每个线程处理的像素块大小
    }
    size_t chunk_width = std::ceil(chunk_width_float);   // 向上取整
    size_t chunk_height = std::ceil(chunk_height_float); // 向上取整

    for(size_t x = 0; x < width; x += chunk_width){
        for(size_t y = 0; y < height; y += chunk_height){
            pending_task_count++; // 增加待执行任务数量
            // 不能超过图像的边界
            if(x + chunk_width > width){
                chunk_width = width - x;
            }
            if(y + chunk_height > height){
                chunk_height = height - y;
            }
            tasks.push(new ParallelForTask(x, y, chunk_width, chunk_height, lambda));
        }
    }
    // 恢复初始分块大小，否则会影响下一次parallelFor，变成小图片
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