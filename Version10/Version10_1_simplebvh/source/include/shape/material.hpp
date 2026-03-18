#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include <glm/glm.hpp>

struct Material {
    glm::vec3 albedo = {1, 1, 1};       // 反射率
    bool is_specular = false;           // 镜面反射/漫反射，默认漫反射
    glm::vec3 emissive = {0, 0, 0};     // 自发光颜色
};


#endif // __MATERIAL_HPP__