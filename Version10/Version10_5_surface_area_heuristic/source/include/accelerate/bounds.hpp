#ifndef __BOUNDS_HPP__
#define __BOUNDS_HPP__

#include "camera/ray.hpp"

struct Bounds {
    Bounds() : b_min(std::numeric_limits<float>::infinity()), b_max(-std::numeric_limits<float>::infinity()) {}  // 退化的包围盒
    Bounds(const glm::vec3 &b_min, const glm::vec3 &b_max) : b_min(b_min), b_max(b_max) {}

    // 把一个点加入包围盒
    void expand(const glm::vec3 &pos) {
        b_min = glm::min(pos, b_min);
        b_max = glm::max(pos, b_max);
    }

    // 光线与包围盒相交测试
    bool hasIntersection(const Ray &ray, float t_min, float t_max) const;
    bool hasIntersection(const Ray &ray, const glm::vec3 &inv_direction,float t_min, float t_max) const;  // 使用预计算的1/direction，优化浮点数除法
    glm::vec3 diagonal() const {return b_max - b_min;}   // 包围盒的对角线
    float area() const {  // 包围盒的表面积
        auto diag = diagonal();
        return (diag.x * diag.y + diag.x * diag.z + diag.y * diag.z) * 2.f;
    }
    
    // 包围盒
    glm::vec3 b_min;
    glm::vec3 b_max;  
};

#endif // __BOUNDS_HPP__