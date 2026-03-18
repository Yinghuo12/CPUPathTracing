#ifndef __SPHERE_HPP__
#define __SPHERE_HPP__

#include "shape.hpp"

// 球体类
struct Sphere:public Shape {
    Sphere(const glm::vec3 &center, float radius): center(center), radius(radius) {}
    // 相交测试
    std::optional<HitInfo> intersect(const Ray& ray, float t_min, float t_max) const override;

    glm::vec3 center;   // 球心坐标
    float radius;       // 半径
};

#endif // __SPHERE_HPP__