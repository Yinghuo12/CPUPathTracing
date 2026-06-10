#include <iostream>
#include <glm/glm.hpp>
#include "camera/film.hpp"

#include "camera/camera.hpp"
#include "shape/sphere.hpp"
#include "shape/model.hpp"
#include "shape/plane.hpp"
#include "shape/scene.hpp"
#include "util/rgb.hpp"
#include "material/diffuse_material.hpp"
#include "material/specular_material.hpp"
#include "material/dielectric_material.hpp"
#include "material/conductor_material.hpp"
#include "material/ground_material.hpp"
#include "renderer/normal_renderer.hpp"
#include "renderer/debug_renderer.hpp"
#include "renderer/path_tracing_renderer.hpp"


int main(){

    Film film{1920, 1080};
    // Film film{192 * 4, 108 * 4};
    Camera camera{film, {-9.5, 1.5, 0}, {0, 0, 0}, 45};

    Model model("models/dragon_871k.obj");
    Sphere sphere{ {0, 0, 0}, 1};
    Plane plane{{0, 0, 0}, {0, 1, 0}};
    
    Scene scene{};
    for (int i = -3; i <= 3; i ++) {
        scene.addShape(
            sphere,
            new DielectricMaterial {
                1.f + 0.2f * (i + 3),
                { 1, 1, 1 },
                (3.f - i) / 18.f,
                (3.f - i) / 6.f,
            },
            { 0, 0.5, i * 2 },
            { 0.8, 0.8, 0.8 }
        );
    }

    for (int i = -3; i <= 3; i ++) {
        glm::vec3 c = RGB::GenerateHeatmapRGB((i + 3.f) / 6.f);
        scene.addShape(
            sphere,
            new ConductorMaterial {
                glm::vec3(2.f - c * 2.f),
                glm::vec3(2.f + c * 3.f),
                (3.f - i) / 6.f,
                (3.f - i) / 18.f
            },
            { 0, 2.5, i * 2 },
            { 0.8, 0.8, 0.8 }
        );
    }
    scene.addShape(
        model,
        new DielectricMaterial { 1.8, RGB(128, 211, 131), 0.4, 0.4 },
        { -5, 0.4, 1.5 },
        { 2, 2, 2 }
    );
    scene.addShape(
        model,
        new ConductorMaterial { { 0.1, 1.2, 1.8 }, { 5, 2.5, 2 }, 0.4, 0.4 },
        { -5, 0.4, -1.5 },
        { 2, 2, 2 }
    );
    scene.addShape(plane, new GroundMaterial { RGB(120, 204, 157) }, { 0, -0.5, 0 });
    auto *light_material = new DiffuseMaterial { { 1, 1, 1 } };
    light_material->setEmissive({ 0.95 * 5, 0.95 * 5, 1 * 5 });
    scene.addShape(sphere, light_material, { -2, 6, 0 }, {2, 2, 2});
    scene.build();    // build BVH

    NormalRenderer normal_render{camera, scene};
    normal_render.render(1, "normal_1.ppm");

    BoundsTestCountRenderer btc_renderer{camera, scene};
    btc_renderer.render(1, "BTC.ppm");
    TriangleTestCountRenderer ttc_renderer{camera, scene};
    ttc_renderer.render(1, "TTC.ppm");

    // 路径追踪
    PathTracingRenderer path_tracing_renderer{camera, scene};
    // path_tracing_renderer.render(128, "PT_microfacet_128.ppm");
    path_tracing_renderer.render(4096, "4096.ppm");

    return 0;
}


// Release Mode  MACMINI_M4
// Profile "Load model models/simple_dragon.obj": 12ms
// Profile "parallelFor": 467ms
// Profile "Save to normal.ppm": 91ms
// Profile "render 1spp normal.ppm": 5524ms
// Profile "render 32spp simple_rt.ppm": 366797ms


// Change parallelFor
// Profile "parallelFor": 1ms

// Change Film::save 
// Profile "Save to simple_rt.ppm": 15ms

// Change complex task
// Profile "Save to simple_rt.ppm": 13ms

// Add Bounds
// Profile "Load model models/simple_dragon.obj": 18ms
// Profile "render 1spp normal.ppm": 1824ms
// Profile "render 32spp simple_rt.ppm": 117272ms

// Load Model - use rapidobj
// Profile "Load model models/simple_dragon.obj": 4ms

// bvh
// Profile "Load model models/simple_dragon.obj": 7ms
// Profile "render 1spp normal_1.ppm": 675ms
// Profile "render 32spp simple_rt_32.ppm": 9846ms

// Dragon 87k
// Profile "Load model models/dragon_87k.obj": 154ms
// Profile "render 1spp normal_1.ppm": 1209ms
// Profile "render 32spp simple_rt_32.ppm": 13616ms

// bvh-flatten
// Profile "render 32spp simple_rt_32.ppm": 12237ms

// 32bit union
// Profile "render 32spp simple_rt_32.ppm": 12885ms

// inv_direction
// Profile "render 32spp simple_rt_32.ppm": 10548ms

// Total node count: 142643
// Leaf node count: 71322
// Triangle count: 87130
// Mean Leaf Node Triangle Count: 1.22164
// Max Leaf Node Triangle Count: 23
// Profile "Load model models/dragon_87k.obj": 189ms

// With SAH
// Total node count: 164067
// Leaf node count: 82034
// Triangle count: 87130
// Mean Leaf Node Triangle Count: 1.06212
// Max Leaf Node Triangle Count: 8
// Profile "Load model models/dragon_87k.obj": 772ms

// With 3Dim SAH
// Total node count: 174195
// Leaf node count: 87098
// Triangle count: 87130
// Mean Leaf Node Triangle Count: 1.00037
// Max Leaf Node Triangle Count: 3
// Profile "Load model models/dragon_87k.obj": 1580ms

// 871k Dragon with 3Dim SAH
// Total node count: 1742423
// Leaf node count: 871212
// Triangle count: 871306
// Mean Leaf Node Triangle Count: 1.00011
// Max Leaf Node Triangle Count: 3
// Profile "Load model models/dragon_871k.obj": 16372ms // 构建bvh需要耗费大量时间
// Profile "render 32spp simple_rt_32.ppm": 13658ms


// buckets_bvh_optimization
// Profile "Load model models/dragon_871k.obj": 3863ms  // 大大减少

// Change ThreadPool
// Lesson10_6_buckets_bvh_optimization
// if(master->tasks.empty()){
//     std::this_thread::sleep_for(std::chrono::milliseconds(2)); // 优化：线程休眠2ms
//     continue;
// }
// Profile "Load model models/dragon_871k.obj": 1845ms   // 耗时减少一半 原先需要跟另一个线程抢资源，另一个线程会耗费掉一半的资源

// BVH Add Allocator
// Profile "Load model models/dragon_871k.obj": 1807ms

// BVH Vector Reserve
// Profile "Load model models/dragon_871k.obj": 1450ms

// render 100 shape instaces scene
// rofile "render 32spp simple_rt_32.ppm": 94640ms


// render 100 shape instances scene with SceneBVH
// Profile "render 32spp simple_rt_32.ppm": 35878ms  // 得到优化

// 解决陷入死循环：SmpleRTRenderer::render 循环结束条件是光线没有与任何物体相交，但是由于浮点数误差，可能导致一根光线进入物体内部，这样不管经过多少次反弹，都不会离开物体的内部，导致死循环
// 解决方案 规定光线弹射最大次数  之后渲染10000个物体：
// Profile "render 32spp simple_rt_32.ppm": 136277ms



// non thread local RNG
//render 128spp:60199ms
// with thread local RNG
//render 128spp:43594ms
