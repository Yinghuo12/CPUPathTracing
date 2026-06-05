#include "renderer/path_tracing_renderer.hpp"
#include "util/frame.hpp"
// #include "sample/spherical.hpp"

glm::vec3 PathTracingRenderer::renderPixel(const glm::ivec3 &pixel_coord) {
    thread_local RNG rng{static_cast<size_t>(pixel_coord.x * 1000000 + pixel_coord.y + pixel_coord.z * 10000000)};  // 每个线程一个RNG实例，避免竞争

    auto ray = camera.generateRay(pixel_coord, { rng.uniform(), rng.uniform() });
    glm::vec3 beta = { 1, 1, 1 };
    glm::vec3 L = { 0, 0, 0 };  // 在像素处感受到的radiance
    float q = 0.9;

    while (true) {
        auto hit_info = scene.intersect(ray);
        if (hit_info.has_value()) {
            L += beta * hit_info->material->emissive;  // 先让光源对像素处的radiance产生贡献

            // 俄罗斯轮盘赌算法
            if (rng.uniform() > q) {
                break;
            }
            beta /= q;

            Frame frame(hit_info->normal);
            glm::vec3 light_direction;
            if(hit_info->material){
                glm::vec3 view_direction = frame.localFromWorld(-ray.direction);
                auto bsdf_sample = hit_info->material->sampleBSDF(hit_info->hit_point, view_direction, rng);
                if(!bsdf_sample.has_value()){
                    break;
                }
                beta *= bsdf_sample->bsdf * glm::abs(bsdf_sample->light_direction.y) / bsdf_sample->pdf;  // beta *= f * cos(theta) / pdf
                light_direction = bsdf_sample->light_direction;
            } else {
                break;
            }

            ray.origin = hit_info->hit_point;
            ray.direction = frame.worldFromLocal(light_direction);
        } else {
            break;
        }
    }

    return L;
}