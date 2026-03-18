#ifndef __RAY_HPP__
#define __RAY_HPP__

#include <glm/glm.hpp>

struct Ray {
    glm::vec3 origin;  // 起点
    glm::vec3 direction;  // 光线方向

    glm::vec3 hit(float t) const {return origin + t * direction;}  // 根据参数t计算光线与物体的交点坐标
};

#endif // __RAY_HPP__