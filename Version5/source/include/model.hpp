#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include "triangle.hpp"
#include <vector>
#include <filesystem>

class Model : public Shape{
public:
    Model(const std::vector<Triangle> &triangles) : triangles(triangles) {}

    Model(const std::filesystem::path &filename);  // 模型加载
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;  // 相交测试

private:
    std::vector<Triangle> triangles;
};


#endif // __MODEL_HPP__