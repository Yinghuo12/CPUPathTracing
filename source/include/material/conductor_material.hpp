#ifndef __CONDUCTOR_MATERIAL_HPP__
#define __CONDUCTOR_MATERIAL_HPP__

#include "material.hpp"

class ConductorMaterial : public Material {
public:
    ConductorMaterial(const glm::vec3 &ior, const glm::vec3 &k) : ior(ior), k(k) {}
    std::optional<BSDFSample> sampleBSDF(const glm::vec3 &hit_point, const glm::vec3 &view_direction, const RNG &rng) const override;

private:
    glm::vec3 ior, k;  // 折射率 吸收率
};

#endif  // __CONDUCTOR_MATERIAL_HPP__