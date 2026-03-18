#include "scene.hpp"
#include <glm/ext/matrix_transform.hpp>

void Scene::addShape(const Shape& shape, const Material &material, const glm::vec3 &pos, const glm::vec3 &scale, const glm::vec3 &rotate) {
    // 根据左乘运算，变换顺序是先缩放后旋转后平移
    glm::mat4 world_from_object =
        glm::translate(glm::mat4(1.f), pos) *
        glm::rotate(glm::mat4(1.f), glm::radians(rotate.z), {0, 0, 1}) *
        glm::rotate(glm::mat4(1.f), glm::radians(rotate.y), {0, 1, 0}) *
        glm::rotate(glm::mat4(1.f), glm::radians(rotate.x), {1, 0, 0}) *
        glm::scale(glm::mat4(1.f), scale);
    instances.push_back(ShapeInstance{shape, material, world_from_object, glm::inverse(world_from_object)});
}

// Scene是物体坐标系，Ray是世界坐标系，因此需要把Ray转换到物体坐标系,再进行求交
std::optional<HitInfo> Scene::intersect(const Ray &ray, float t_min, float t_max) const {
   std::optional<HitInfo> closest_hit_info{};  // 记录最近的交点信息
   const ShapeInstance *closest_instance = nullptr;  // 记录最近的物体

    for (const auto &instance : instances) {  // 遍历所有物体
        auto ray_object = ray.objectFromWorld(instance.object_from_world);    // 把Ray转换到物体坐标系
        auto hit_info = instance.shape.intersect(ray_object, t_min, t_max);  // 求交
        if (hit_info.has_value()) {
           closest_hit_info = hit_info;  // 第一次命中先记录交点信息，后续命中则更新
           t_max = hit_info->t; // 更新t_max，避免后面的物体被遮挡
           closest_instance = &instance;  // 记录最近的物体
        }
    }

    if(closest_instance){
        // 把交点信息转换回世界坐标系
        closest_hit_info->hit_point = closest_instance->world_from_object * glm::vec4(closest_hit_info->hit_point, 1.f);
        closest_hit_info->normal = glm::normalize(glm::vec3(glm::transpose(glm::inverse(closest_instance->world_from_object)) * glm::vec4(closest_hit_info->normal, 0.f)));
        closest_hit_info->material = &closest_instance->material;  // 记录最近实例材质
    }

    return closest_hit_info;
}
