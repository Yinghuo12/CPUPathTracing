#ifndef __DIELECTRIC_MATERIAL_HPP__
#define __DIELECTRIC_MATERIAL_HPP__

#include "material.hpp"
#include "microfacet_theory.hpp"

class DielectricMaterial : public Material {
public:
    DielectricMaterial(float ior, const glm::vec3 &albedo, float alpha_x = 0, float alpha_z = 0): ior(ior), albedo_r(albedo), albedo_t(albedo), microfacet_theory(alpha_x, alpha_z) {}
    DielectricMaterial(float ior, const glm::vec3 &albedo_r, const glm::vec3 &albedo_t, float alpha_x = 0, float alpha_z = 0): ior(ior), albedo_r(albedo_r), albedo_t(albedo_t), microfacet_theory(alpha_x, alpha_z) {}

    std::optional<BSDFSample> sampleBSDF(const glm::vec3 &hit_point, const glm::vec3 &view_direction, const RNG &rng) const override;

private:
    float ior;  // 折射率
    glm::vec3 albedo_r, albedo_t;  // 反射和透射的反照率
    MicrofacetTheory microfacet_theory;
};

#endif  // __DIELECTRIC_MATERIAL_HPP__    