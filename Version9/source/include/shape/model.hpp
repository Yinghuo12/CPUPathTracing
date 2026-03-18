#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include "accelerate/bounds.hpp"
#include "triangle.hpp"
#include <vector>
#include <filesystem>

class Model : public Shape{
public:
    Model(const std::vector<Triangle> &triangles) : triangles(triangles) {build();}

    Model(const std::filesystem::path &filename);  // 模型加载
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;  // 相交测试

private:
    void build();  // 构建包围盒
private:
    std::vector<Triangle> triangles;
    Bounds bounds{};
};


#endif // __MODEL_HPP__