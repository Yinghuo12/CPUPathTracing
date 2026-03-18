#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include "accelerate/bvh.hpp"
#include "accelerate/bounds.hpp"
#include "triangle.hpp"
#include <vector>
#include <filesystem>

class Model : public Shape{
public:
    Model(const std::vector<Triangle> &triangles) {
        auto ts = triangles;
        bvh.build(std::move(ts));}

    Model(const std::filesystem::path &filename);  // 模型加载
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;  // 相交测试
    Bounds getBounds() const override{ return bvh.getBounds();};  // 获取包围盒

private:
    BVH bvh{};
};


#endif // __MODEL_HPP__