#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "shape.hpp"
#include "../accelerate/scene_bvh.hpp"

struct Scene : public Shape {
public:

    // 假设所有的shape都定义在自己的对象空间中，当添加到世界空间的场景中时，需要指定模型摆放的位置，缩放和旋转
    void addShape(const Shape &shape, 
        const Material *material = nullptr,
        const glm::vec3 &pos = {0, 0, 0},
        const glm::vec3 &scale = {1, 1, 1}, 
        const glm::vec3 &rotate = {0, 0, 0}
    ); 

    std::optional<HitInfo> intersect(
        const Ray& ray,
        float t_min = 1e-5, 
        float t_max = std::numeric_limits<float>::infinity()
    )const override;

    void build(){
        scene_bvh.build(std::move(instances));
    }
private:
    std::vector<ShapeInstance> instances;  // 场景的形状就是场景内所包含的所有的物体的形状的总和
    SceneBVH scene_bvh {};
};

#endif // __SCENE_HPP__