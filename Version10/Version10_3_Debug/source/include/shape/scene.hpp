#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "shape.hpp"

// shape的指针和转换矩阵存储在一起(形状和变换是一起存储的)
struct ShapeInstance{
    const Shape & shape;
    Material material;
    glm::mat4 world_from_object;
    glm::mat4 object_from_world; // 逆变换矩阵
};

struct Scene : public Shape {
public:

    // 假设所有的shape都定义在自己的对象空间中，当添加到世界空间的场景中时，需要指定模型摆放的位置，缩放和旋转
    void addShape(const Shape &shape, 
        const Material &material = {},
        const glm::vec3 &pos = {0, 0, 0},
        const glm::vec3 &scale = {1, 1, 1}, 
        const glm::vec3 &rotate = {0, 0, 0}
    ); 

    std::optional<HitInfo> intersect(
        const Ray& ray,
        float t_min = 1e-5, 
        float t_max = std::numeric_limits<float>::infinity()
    )const override;

private:
    std::vector<ShapeInstance> instances;  // 场景的形状就是场景内所包含的所有的物体的形状的总和
};

#endif // __SCENE_HPP__