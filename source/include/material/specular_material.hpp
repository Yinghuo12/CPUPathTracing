#ifndef __SPECULAR_MATERIAL_HPP__
#define __SPECULAR_MATERIAL_HPP__

#include "material.hpp"

class SpecularMaterial : public Material {
public:
    SpecularMaterial(const glm::vec3 &albedo) : albedo(albedo) {}
    // BSDF = BRDF + BTDF (双向散射分布函数 = 双向反射分布函数 + 双向透射分布函数) 
    std::optional<BSDFSample> sampleBSDF(const glm::vec3 &hit_point, const glm::vec3 &view_direction, const RNG &rng) const override;
private:
    glm::vec3 albedo {};
};

#endif // __SPECULAR_MATERIAL_HPP__