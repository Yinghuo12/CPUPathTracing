#ifndef __RAY_HPP__
#define __RAY_HPP__

#include <glm/glm.hpp>

struct Ray {
    glm::vec3 origin;  // 起点
    glm::vec3 direction;  // 光线方向

    glm::vec3 hit(float t) const {return origin + t * direction;}  // 根据参数t计算光线与物体的交点坐标

    // 光线从世界坐标系转换到物体坐标系，用于scene中的求交函数
    Ray objectFromWorld(const glm::mat4 object_from_world) const;

};

struct HitInfo{
    float t;  // 光线与物体的交点距离
    glm::vec3 hit_point;  // 光线与物体的交点坐标
    glm::vec3 normal;  // 光线与物体交点处的法线
};

#endif // __RAY_HPP__