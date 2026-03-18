#ifndef __BVH_HPP__
#define __BVH_HPP__

#include "bounds.hpp"
#include "../shape/triangle.hpp"

struct BVHTreeNode {
    Bounds bounds {};
    std::vector<Triangle> triangles;
    BVHTreeNode *children[2];
    size_t depth;

    void updateBounds() {
        bounds = {};
        for (const auto &triangle : triangles) {
            bounds.expand(triangle.p0);
            bounds.expand(triangle.p1);
            bounds.expand(triangle.p2);
        }
    }
};

// 线性节点  32字节对齐
struct alignas(32) BVHNode{
    Bounds bounds {};
    union{
        int child1_index;
        int triangle_index;
    };
    uint16_t triangle_count;
    uint8_t depth;  // 用于debug
};


class BVH : public Shape{
public:
    void build(std::vector<Triangle> &&triangles);
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
private:
    void recursiveSplit(BVHTreeNode *node);
    size_t recursiveFlatten(BVHTreeNode *node);
private:
    std::vector<BVHNode> nodes;
    std::vector<Triangle> ordered_triangles;
};


#endif  // __BVH_HPP__