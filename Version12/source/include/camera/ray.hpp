#ifndef __RAY_HPP__
#define __RAY_HPP__

#include <glm/glm.hpp>
#include "../shape/material.hpp"
#include "../util/debug_macro.hpp"

struct Ray {
    glm::vec3 origin;  // 起点
    glm::vec3 direction;  // 光线方向

    glm::vec3 hit(float t) const {return origin + t * direction;}  // 根据参数t计算光线与物体的交点坐标

    // 光线从世界坐标系转换到物体坐标系，用于scene中的求交函数
    Ray objectFromWorld(const glm::mat4 object_from_world) const;

    // 从hitinfo中转移到ray:因为我们希望即使光线没有与物体产生交点，我们也能获取到调试信息  新增mutable: 因为ray都是用常量引用传入
    DEBUG_LINE(mutable size_t bounds_test_count = 0)  // 光线与物体求交过程中，包围盒求交的次数
    DEBUG_LINE(mutable size_t triangle_test_count = 0)  // 光线与物体求交过程中，三角形求交的次数
};

struct HitInfo{
    float t;  // 光线与物体的交点距离
    glm::vec3 hit_point;  // 光线与物体的交点坐标
    glm::vec3 normal;  // 光线与物体交点处的法线
    const Material *material = nullptr;  // 光线与物体交点处的材质
};

#endif // __RAY_HPP__