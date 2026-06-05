#ifndef __DIFFUSE_MATERIAL_HPP__
#define __DIFFUSE_MATERIAL_HPP__

#include "material.hpp"

class DiffuseMaterial : public Material {
public:
    DiffuseMaterial(const glm::vec3 &albedo) : albedo(albedo) {}
    std::optional<BSDFSample> sampleBSDF(const glm::vec3 &hit_point, const glm::vec3 &view_direction, const RNG &rng) const override;
private:
    glm::vec3 albedo {};
};

#endif // __DIFFUSE_MATERIAL_HPP__