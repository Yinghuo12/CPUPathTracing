#include <iostream>
#include <glm/glm.hpp>
#include "thread_pool.hpp"
#include "film.hpp"

#include "camera.hpp"
#include "sphere.hpp"

class SimpleTask: public Task{
public:
    void run() override{
        std::cout << "Hello World!" << std::endl;
    }
};

int main(){

    ThreadPool thread_pool{};
    Film film{1920, 1080};
    Camera camera{film, {0, 0, 1}, {0, 0, 0}, 90};

    Sphere sphere{{0, 0, 0}, 0.5f};

    // glm::vec3 light_pos{3, 3, -2}; // 光源    
    glm::vec3 light_pos{1, 1, 1}; // 光源

    // 线程池并行for循环每一个像素，后续计算像素的值
    thread_pool.parallelFor(film.getWidth(), film.getHeight(), [&](size_t x, size_t y){
        auto ray = camera.generateRay({x, y});  // 每一个像素用相机生成一个光线
        auto result = sphere.intersect(ray);   // 相交测试
        if(result.has_value()){
            auto hit_point = ray.hit(result.value());  //  计算交点坐标
            auto normal = glm::normalize(hit_point - sphere.center);  // 计算交点处的法线
            auto L = glm::normalize(light_pos - hit_point);  // 计算交点处的入射光线方向
            float cosine = glm::max(0.f, glm::dot(normal, L));  // 计算入射光线与法线的夹角余弦值

            film.setPixel(x, y, {cosine, cosine, cosine});  // 根据夹角余弦值设置像素颜色，简单的Lambertian反射模型
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