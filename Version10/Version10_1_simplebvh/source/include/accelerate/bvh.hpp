#ifndef __BVH_HPP__
#define __BVH_HPP__

#include "bounds.hpp"
#include "../shape/triangle.hpp"

struct BVHNode {
    Bounds bounds {};
    std::vector<Triangle> triangles;
    BVHNode *children[2];

    void updateBounds() {
        bounds = {};
        for (const auto &triangle : triangles) {
            bounds.expand(triangle.p0);
            bounds.expand(triangle.p1);
            bounds.expand(triangle.p2);
        }
    }
};


class BVH : public Shape{
public:
    void build(std::vector<Triangle> &&triangles);
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
private:
    void recursiveSplit(BVHNode *node);
    void recursiveIntersect(BVHNode *node, const Ray &ray, float t_min, float t_max, std::optional<HitInfo> &closest_hit_info) const;
private:
    BVHNode *root;
};


#endif  // __BVH_HPP__