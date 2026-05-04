#ifndef __SPHERICAL_HPP__
#define __SPHERICAL_HPP__

#include "util/rng.hpp"
#include <glm/glm.hpp>

constexpr float PI = 3.14159265359;

// 均匀采样圆盘
inline glm::vec2 UniformSampleUnitDisk(const glm::vec2 &u) {  // 参数是两个均匀分布的随机变量
    float r = glm::sqrt(u.x);
    float theta = 2 * PI * u.y;
    return { r * glm::cos(theta), r * glm::sin(theta) };   // 转换为笛卡尔坐标系下的坐标
}

// 余弦重要性采样
inline glm::vec3 CosineSampleHemisphere(const glm::vec2 &u) {
    float r = glm::sqrt(u.x);
    float phi = 2 * PI * u.y;
    return { r * glm::cos(phi), glm::sqrt(1 - r * r), r * glm::sin(phi) };
}

// 接受拒绝采样
inline glm::vec3 UniformSampleHemisphere(const RNG &rng) {
    glm::vec3 result;
    do {  // 随机生成一个单位球内的方向
        result = {rng.uniform(), rng.uniform(), rng.uniform()};  // 随机生成一个方向
        result = result * 2.f - glm::vec3(1, 1, 1);     // 将方向映射到单位球内
    } while(glm::length(result) > 1);  // 如果生成的方向不在单位球内，则重新生成
    if(result.y < 0){                  // 如果生成的方向在单位球下半球，则取反, 保证镜面反射的特点
        result.y = -result.y;
    }
    return glm::normalize(result);
}

#endif // __SPHERICAL_HPP__