#include "renderer/debug_renderer.hpp"
#include "util/rgb.hpp"

glm::vec3 BoundsTestCountRenderer::renderPixel(const glm::ivec2 &pixel_coord) {
#ifdef WITH_DEBUG_INFO
    auto ray = camera.generateRay(pixel_coord);
    auto hit_info = scene.intersect(ray);
    if (hit_info.has_value()) {
        return RGB::GenerateHeatmapRGB(hit_info->bounds_test_count / 200.f);
    }
    return {}; // 返回黑色
#else
    return {};
#endif
}

glm::vec3 TriangleTestCountRenderer::renderPixel(const glm::ivec2 &pixel_coord) {
#ifdef WITH_DEBUG_INFO
    auto ray = camera.generateRay(pixel_coord);
    auto hit_info = scene.intersect(ray);
    if (hit_info.has_value()) {
        return RGB::GenerateHeatmapRGB(hit_info->triangle_test_count / 20.f);
    }
    return {};
#else
    return {};
#endif
}

glm::vec3 BoundsDepthRenderer::renderPixel(const glm::ivec2 &pixel_coord) {
#ifdef WITH_DEBUG_INFO
    auto ray = camera.generateRay(pixel_coord);
    auto hit_info = scene.intersect(ray);
    if (hit_info.has_value()) {
        return RGB::GenerateHeatmapRGB(hit_info->bounds_depth / 32.f);  // 栈的最大值就是32
    }
    return {};
#else
    return {};
#endif
}