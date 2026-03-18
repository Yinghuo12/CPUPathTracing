#ifndef __SPHERE_HPP__
#define __SPHERE_HPP__

#include "ray.hpp"
#include <optional>

// 球体类
struct Sphere{

    // 相交测试
    std::optional<float> intersect(const Ray& ray) const;

    glm::vec3 center;   // 球心坐标
    float radius;       // 半径
};

#endif // __SPHERE_HPP__