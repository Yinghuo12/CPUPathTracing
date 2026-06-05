#include "material/specular_material.hpp"

std::optional<BSDFSample> SpecularMaterial::sampleBSDF(const glm::vec3 &hit_point, const glm::vec3 &view_direction, const RNG &rng) const {
    glm::vec3 light_direction {-view_direction.x, view_direction.y, -view_direction.z};  // 镜面反射
    glm::vec3 bsdf = albedo / glm::abs(light_direction.y);  // 理论上应该除以|cos(theta)|，但由于light_direction是镜面反射，所以cos(theta) = light_direction.y
    float pdf = 1;  // delta分布，pdf为1
    return BSDFSample {bsdf, pdf, light_direction};
}