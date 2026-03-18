// #include "renderer/simple_rt_renderer.hpp"
// #include "util/frame.hpp"
// #include "sample/spherical.hpp"

// glm::vec3 SimpleRTRenderer::renderPixel(const glm::ivec2 &pixel_coord){
//     auto ray = camera.generateRay(pixel_coord, {rng.uniform(), rng.uniform()}); // 每一个像素用相机生成一个光线, 同时加入随机偏移量
//     glm::vec3 beta = {1, 1, 1};  // 总反照率
//     glm::vec3 color = {0, 0, 0};   // 最终像素感受到的光线亮度
//     size_t max_bounce_count = 32;  // 解决光线进入物体内部导致死循环问题

//     while(max_bounce_count--){
//         // Lesson7: 这里替换为简易光线追踪算法的实现
//         auto hit_info = scene.intersect(ray);   // 判断光线与场景有没有交点
//         if(hit_info.has_value()){
//             color += beta * hit_info->material->emissive;  // 计算光源对像素的贡献(反照率*材质自发光)
//             beta *= hit_info->material->albedo;            // 计算光线在材质上的反射(反照率*命中点反照率)

//             ray.origin = hit_info->hit_point;  // 更新光线起点

//             Frame frame(hit_info->normal);  // 计算局部反射空间
//             glm::vec3 light_direction;      // 计算光源方向
//             if(hit_info->material->is_specular){  // 镜面反射
//                 glm::vec3 view_direction = frame.localFromWorld(-ray.direction);  // 观察方向就是入射方向取反
//                 light_direction = {-view_direction.x, view_direction.y, -view_direction.z};  // 镜面反射方向就是观察方向的x,z取反
//             } else {   // 漫反射
//                 light_direction = UniformSampleHemisphere(rng);  // 随机生成一个反射方向
//             }
//             ray.direction = frame.worldFromLocal(light_direction);  // 将反射方向转换到世界坐标系
//         } else {
//             break;
//         }
//     }
    
//     return color;
// }