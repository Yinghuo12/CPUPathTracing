#ifndef __BOUNDS_HPP__
#define __BOUNDS_HPP__

#include "camera/ray.hpp"

struct Bounds {
    Bounds() : b_min(0), b_max(0) {}
    Bounds(const glm::vec3 &b_min, const glm::vec3 &b_max) : b_min(b_min), b_max(b_max) {}

    // 把一个点加入包围盒
    void expand(const glm::vec3 &pos) {
        b_min = glm::min(pos, b_min);
        b_max = glm::max(pos, b_max);
    }

    // 光线与包围盒相交测试
    bool hasIntersection(const Ray &ray, float t_min, float t_max) const;

    glm::vec3 b_min, b_max;  // 包围盒
};

#endif // __BOUNDS_HPP__