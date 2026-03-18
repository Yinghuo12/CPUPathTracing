#ifndef __TRIANGLE_HPP__
#define __TRIANGLE_HPP__

#include "shape.hpp"

struct Triangle:public Shape {
    // 构造函数, 可以设置顶点法线
    Triangle(
        const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2,
        const glm::vec3 &n0, const glm::vec3 &n1, const glm::vec3 &n2
    ): p0(p0), p1(p1), p2(p2), n0(n0), n1(n1), n2(n2) {}

    // 默认构造函数，顶点法线设置为面法线
    Triangle(
        const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2
    ): p0(p0), p1(p1), p2(p2) {
        // 计算顶点法线，简单地把它们设置为三角形的面法线
        glm::vec3 e1 = p1 - p0;
        glm::vec3 e2 = p2 - p0;
        glm::vec3 normal = glm::normalize(glm::cross(e1, e2));
        n0 = normal;
        n1 = normal;
        n2 = normal;
    }
    
    std::optional<HitInfo> intersect(const Ray& ray, float t_min, float t_max) const override;
    // 获取三角形包围盒 
    Bounds getBounds() const override{
        Bounds bounds{};
        bounds.expand(p0);
        bounds.expand(p1);
        bounds.expand(p2);
        return bounds;
    }

    glm::vec3 p0, p1, p2;  // 三角形的三个顶点坐标
    glm::vec3 n0, n1, n2;  // 三角形的三个顶点法线
};

#endif // __TRIANGLE_HPP__