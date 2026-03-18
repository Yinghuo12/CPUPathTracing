#include <iostream>
#include <glm/glm.hpp>
#include "thread_pool.hpp"
#include "film.hpp"


int main(){

    Film film{1920, 1080};

    ThreadPool thread_pool{};

    thread_pool.parallelFor(200, 100, [&](size_t x, size_t y){
        film.setPixel(x, y, {0.5, 0.7, 0.2});
    });
    thread_pool.wait();

    // 测试保存胶片的时间
    auto now = std::chrono::high_resolution_clock::now();
    film.save("test.ppm");
    auto time = std::chrono::high_resolution_clock::now() - now;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
    std::cout << "Time: " << ms.count() << "ms" << std::endl;

    return 0;
}