#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "util/rng.hpp"
#include <glm/glm.hpp>

class Material {
public:
    virtual glm::vec3 sampleBSDF(const glm::vec3 &hit_point,const glm::vec3 &view_direction, glm::vec3 &beta, const RNG &rng) const = 0;
    void setEmissive(const glm::vec3 &emissive) { this->emissive = emissive; }  // 设置自发光
public:
    glm::vec3 emissive;
};

#endif // __MATERIAL_H__