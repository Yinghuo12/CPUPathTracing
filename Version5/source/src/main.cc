#include <iostream>
#include <glm/glm.hpp>
#include "thread_pool.hpp"
#include "film.hpp"

#include "camera.hpp"
#include "sphere.hpp"
#include "model.hpp"

#include <iostream>

class SimpleTask: public Task{
public:
    void run() override{
        std::cout << "Hello World!" << std::endl;
    }
};

int main(){
    ThreadPool thread_pool{};

    Film film{1920, 1080};
    Camera camera{film, {-0.6, 0, 0}, {0, 0, 0}, 90};
    std::atomic<int> count = 0;  // 测试线程池的并行性能，统计并行执行的任务数量

    Model model("models/simple_dragon.obj");

    Sphere sphere{{0, 0, 0}, 0.5f};
    // Shape &shape = sphere;  // 测试多态
    Shape &shape = model;

    glm::vec3 light_pos{-1, 2, 1}; // 光源

    // 线程池并行for循环每一个像素，后续计算像素的值
    thread_pool.parallelFor(film.getWidth(), film.getHeight(), [&](size_t x, size_t y){
        auto ray = camera.generateRay({x, y});  // 每一个像素用相机生成一个光线
        auto hit_info = shape.intersect(ray);   // 相交测试
        if(hit_info.has_value()){
            auto L = glm::normalize(light_pos - hit_info->hit_point);  // 计算交点处的入射光线方向
            float cosine = glm::max(0.f, glm::dot(hit_info->normal, L));  // 计算入射光线与法线的夹角余弦值

            film.setPixel(x, y, {cosine, cosine, cosine});  // 根据夹角余弦值设置像素颜色，简单的Lambertian反射模型
            // film.setPixel(x, y, {1, 1, 1});  // 白色 
        }

        count++; // 测试线程池的并行性能，统计并行执行的任务数量
        if(count % film.getWidth() == 0){ // 每当处理完一行像素时，输出当前的进度
            std::cout << static_cast<float>(count) / (film.getWidth() * film.getHeight()) << std::endl; 
        }
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