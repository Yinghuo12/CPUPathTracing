
#include "material/dielectric_material.hpp"

// snell定律fresnel定律
float Fresnel(float etai_div_etat, float cos_theta_t, float &cos_theta_i) {
    float sin2_theta_t = 1 - cos_theta_t * cos_theta_t;
    float sin2_theta_i = sin2_theta_t / (etai_div_etat * etai_div_etat);

    // 全反射
    if (sin2_theta_i >= 1) {
        return 1;
    }

    cos_theta_i = glm::sqrt(1 - sin2_theta_i);
    float r_parl = (cos_theta_i - etai_div_etat * cos_theta_t) / (cos_theta_i + etai_div_etat * cos_theta_t);
    float r_perp = (etai_div_etat * cos_theta_i - cos_theta_t) / (etai_div_etat * cos_theta_i + cos_theta_t);
    return 0.5 * (r_parl * r_parl + r_perp * r_perp);
}

std::optional<BSDFSample> DielectricMaterial::sampleBSDF(const glm::vec3 &hit_point, const glm::vec3 &view_direction, const RNG &rng) const {
    if(ior == 1) {
        return BSDFSample { albedo_t / glm::abs(view_direction.y), 1, -view_direction };  // 折射率为1时，直接返回入射方向
    }
    float etai_div_etat = ior;
    glm::vec3 microfacet_normal = { 0, 1, 0 };
    if(!microfacet_theory.isDeltaDistibution()) {
        microfacet_normal = microfacet_theory.sampleVisibleNormal(view_direction, rng);
    }
    float cos_theta_t = view_direction.y;
    float scale = 1.f;
    // 观察方向位于下方
    if (cos_theta_t < 0) {
        etai_div_etat = 1.f / ior;
        scale = -1;
        cos_theta_t = -cos_theta_t;
    }

    float cos_theta_i;
    float fr = Fresnel(etai_div_etat, cos_theta_t, cos_theta_i);

    // 根据Fresnel采样反射与透射
    // 反射
    if (rng.uniform() <= fr) {
        glm::vec3 light_direction = -view_direction + 2.f * glm::dot(microfacet_normal, view_direction) * microfacet_normal; // 镜面反射
        if(microfacet_theory.isDeltaDistibution()) {
            return BSDFSample {albedo_r / glm::abs(light_direction.y), 1, light_direction };  // 理论上应该除以|cos(theta)|，但由于light_direction是镜面反射，所以cos(theta) = light_direction.y
        }
        glm::vec3 brdf = albedo_r * microfacet_theory.normalDistribution(microfacet_normal)
            * microfacet_theory.heightCorrelatedMaskingShadowing(light_direction, view_direction, microfacet_normal)
            / glm::abs(4.f * light_direction.y * view_direction.y);
        float pdf = microfacet_theory.visibleNormalDistribution(view_direction, microfacet_normal) / glm::abs(4.f * glm::dot(view_direction, microfacet_normal));
        return BSDFSample { brdf, pdf, light_direction };
    // 透射
    } else {
        glm::vec3 light_direction {(-view_direction / etai_div_etat) + (cos_theta_t / etai_div_etat - cos_theta_i) * scale * microfacet_normal};  // 计算透射方向
        float det_J = etai_div_etat * etai_div_etat * glm::abs(glm::dot(light_direction, microfacet_normal)) 
            / glm::pow(
                glm::abs(glm::dot(view_direction, microfacet_normal)) - etai_div_etat * etai_div_etat * glm::abs(glm::dot(light_direction, microfacet_normal))
                , 2
            );
        glm::vec3 btdf = albedo_t * det_J * microfacet_theory.normalDistribution(microfacet_normal)
            * microfacet_theory.heightCorrelatedMaskingShadowing(light_direction, view_direction, microfacet_normal)
            * glm::abs(glm::dot(view_direction, microfacet_normal) / (light_direction.y * view_direction.y));
        float pdf = microfacet_theory.visibleNormalDistribution(view_direction, microfacet_normal) * det_J;
        return BSDFSample { btdf / (etai_div_etat * etai_div_etat), pdf, light_direction };
    }
}