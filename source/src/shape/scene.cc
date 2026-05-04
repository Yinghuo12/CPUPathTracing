#include "shape/scene.hpp"
#include <glm/ext/matrix_transform.hpp>

void Scene::addShape(const Shape& shape, const Material *material, const glm::vec3 &pos, const glm::vec3 &scale, const glm::vec3 &rotate) {
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
    return scene_bvh.intersect(ray, t_min, t_max);
}
