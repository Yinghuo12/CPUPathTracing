#ifndef __PLANE_H__
#define __PLANE_H__

#include "shape.hpp"

struct Plane: public Shape{
    Plane(const glm::vec3& point, const glm::vec3& normal) : point(point), normal(normalize(normal)) {}

    std::optional<HitInfo> intersect(const Ray& ray, float t_min, float t_max) const override;

    glm::vec3 point;
    glm::vec3 normal;

};

#endif // __PLANE_H__