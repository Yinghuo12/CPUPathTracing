#include <iostream>
#include <glm/glm.hpp>
#include "camera/film.hpp"

#include "camera/camera.hpp"
#include "shape/sphere.hpp"
#include "shape/model.hpp"
#include "shape/plane.hpp"
#include "shape/scene.hpp"
#include "util/rgb.hpp"
#include "renderer/normal_renderer.hpp"
#include "renderer/simple_rt_renderer.hpp"
#include "renderer/debug_renderer.hpp"


int main(){

    Film film{1920, 1080};
    Camera camera{film, {-3.6, 0, 0}, {0, 0, 0}, 45};

    Model model("models/dragon_87k.obj");
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
        {3, 3, 3}
    );
    // scene.addShape(
    //     sphere, 
    //     {{1, 1, 1}, false, RGB(255, 128, 128)}, 
    //     {0, 0, 2.5}
    // );
    // scene.addShape(
    //     sphere, 
    //     {{1, 1, 1}, false, RGB(128, 128, 255)}, 
    //     {0, 0, -2.5}
    // );
    // scene.addShape(
    //     sphere, 
    //     {{1, 1, 1}, true}, 
    //     {3, 0.5, -2}
    // );
    // scene.addShape(
    //     plane, 
    //     // {},
    //     {RGB(120, 204, 157)},
    //     {0, -0.5, 0}
    // );

    NormalRenderer normal_render{camera, scene};
    normal_render.render(1, "normal_1.ppm");

    BoundsTestCountRenderer btc_renderer{camera, scene};
    btc_renderer.render(1, "BTC.ppm");
    TriangleTestCountRenderer ttc_renderer{camera, scene};
    ttc_renderer.render(1, "TTC.ppm");
    BoundsDepthRenderer bd_renderer{camera, scene};
    bd_renderer.render(1, "BD.ppm");

    SimpleRTRenderer simple_rt_renderer{camera, scene};
    simple_rt_renderer.render(32, "simple_rt_32.ppm");

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
