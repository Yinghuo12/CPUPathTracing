#include <iostream>
#include <glm/glm.hpp>
#include "thread_pool.hpp"
#include "film.hpp"

#include "camera.hpp"
#include "sphere.hpp"
#include "model.hpp"
#include "plane.hpp"
#include "scene.hpp"
#include "frame.hpp"
#include "rgb.hpp"

#include <random>
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
    Camera camera{film, {-3.6, 0, 0}, {0, 0, 0}, 45};
    std::atomic<int> count = 0;  // 测试线程池的并行性能，统计并行执行的任务数量

    Model model("models/simple_dragon.obj");
    Sphere sphere{
        {0, 0, 0}, 
        1
    };
    Plane plane{
        {0, 0, 0}, 
        {0, 1, 0}
    };


    Scene scene{};
    scene.addShape(
        model,
        // {},
        {RGB(202, 159, 117)},
        {0, 0, 0}, 
        {1, 3, 2}
    );
    scene.addShape(
        sphere, 
        {{1, 1, 1}, false, RGB(255, 128, 128)}, 
        {0, 0, 2.5}
    );
    scene.addShape(
        sphere, 
        {{1, 1, 1}, false, RGB(128, 128, 255)}, 
        {0, 0, -2.5}
    );
    scene.addShape(
        sphere, 
        {{1, 1, 1}, true}, 
        {3, 0.5, -2}
    );
    scene.addShape(
        plane, 
        // {},
        {RGB(120, 204, 157)},
        {0, -0.5, 0}
    );


    std::mt19937 gen(23451334);   // 随机数生成器
    std::uniform_real_distribution<float> uniform(-1, 1);  // 随机数分布

    // 每个像素有多少个采样点
    // int spp = 1;  // debug
    int spp = 128; // 高质量渲染

    // 线程池并行for循环每一个像素，后续计算像素的值
    thread_pool.parallelFor(film.getWidth(), film.getHeight(), [&](size_t x, size_t y){
        for(int i = 0; i < spp; i++){
            
            auto ray = camera.generateRay({x, y}, {abs(uniform(gen)), abs(uniform(gen))});  // 每一个像素用相机生成一个光线, 同时加入随机偏移量

            glm::vec3 beta = {1, 1, 1};  // 总反照率
            glm::vec3 color = {0, 0, 0};   // 最终像素感受到的光线亮度

            while(true){
                // Lesson7: 这里替换为简易光线追踪算法的实现
                auto hit_info = scene.intersect(ray);   // 判断光线与场景有没有交点
                if(hit_info.has_value()){
                    color += beta * hit_info->material->emissive;  // 计算光源对像素的贡献(反照率*材质自发光)
                    beta *= hit_info->material->albedo;            // 计算光线在材质上的反射(反照率*命中点反照率)

                    ray.origin = hit_info->hit_point;  // 更新光线起点

                    Frame frame(hit_info->normal);  // 计算局部反射空间
                    glm::vec3 light_direction;      // 计算光源方向
                    if(hit_info->material->is_specular){  // 镜面反射
                        glm::vec3 view_direction = frame.localFromWorld(-ray.direction);  // 观察方向就是入射方向取反
                        light_direction = {-view_direction.x, view_direction.y, -view_direction.z};  // 镜面反射方向就是观察方向的x,z取反
                    } else {   // 漫反射
                        do {  // 随机生成一个单位球内的方向
                            light_direction = {uniform(gen), uniform(gen), uniform(gen)};  // 随机生成一个方向
                        } while(glm::length(light_direction) > 1);  // 如果生成的方向不在单位球内，则重新生成
                        if(light_direction.y < 0){                  // 如果生成的方向在单位球下半球，则取反, 保证镜面反射的特点
                            light_direction.y = -light_direction.y;
                        }
                    }
                    ray.direction = frame.worldFromLocal(light_direction);  // 将反射方向转换到世界坐标系
                } else{
                    break;
                }
            }

            // if(hit_info.has_value()){
                // 被光线追踪算法替W
                // auto L = glm::normalize(light_pos - hit_info->hit_point);  // 计算交点处的入射光线方向
                // float cosine = glm::max(0.f, glm::dot(hit_info->normal, L));  // 计算入射光线与法线的夹角余弦值
                // film.setPixel(x, y, {cosine, cosine, cosine});  // 根据夹角余弦值设置像素颜色，简单的Lambertian反射模型  设置为{1，1,1}则为白色
            // }

            film.addSample(x, y, color);  // 设置像素颜色
        }

        int n = ++count; // 测试线程池的并行性能，统计并行执行的任务数量
        if(n % film.getWidth() == 0){ // 每当处理完一行像素时，输出当前的进度
            std::cout << static_cast<float>(n) / (film.getWidth() * film.getHeight()) << std::endl; 
        }
    });

    thread_pool.wait();

    // 测试保存胶片的时间
    auto now = std::chrono::high_resolution_clock::now();
    film.save("test_spp128.ppm");
    auto time = std::chrono::high_resolution_clock::now() - now;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
    std::cout << "Time: " << ms.count() << "ms" << std::endl;

    return 0;
}