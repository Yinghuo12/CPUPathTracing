#include "accelerate/bvh.hpp"

void BVH::build(std::vector<Triangle> &&triangles) {
    // 创建BVH的根节点
    root = new BVHNode{};
    // 使用移动语义将三角形数据转移到根节点中，避免不必要的拷贝
    root->triangles = std::move(triangles);
    // 更新根节点的包围盒，使其包含所有三角形
    root->updateBounds();
    // 递归地分割BVH节点，构建层次结构
    recursiveSplit(root);
}

void BVH::recursiveSplit(BVHNode *node) {
    // 如果当前节点只有一个三角形，则无需继续分割
    if (node->triangles.size() == 1) {
        return;
    }

    // 计算当前节点包围盒的对角线长度，并确定最长的轴
    auto diag = node->bounds.diagonal();
    // 比较三个轴的长度，找出最长的轴
    // 0表示x轴，1表示y轴，2表示z轴
    size_t max_axis = diag.x > diag.y ? (diag.x > diag.z ? 0 : 2) : (diag.y > diag.z ? 1 : 2);
    // 计算最长轴的中点位置
    float mid = node->bounds.b_min[max_axis] + diag[max_axis] * 0.5f;

    // 创建两个子节点的三角形列表
    std::vector<Triangle> child0_triangles, child1_triangles;
    // 遍历当前节点的所有三角形，根据三角形中心点在最长轴上的位置分配到两个子节点
    for (const auto &triangle : node->triangles) {
        // 计算三角形的中心点坐标
        if ((triangle.p0[max_axis] + triangle.p1[max_axis] + triangle.p2[max_axis]) / 3.f < mid) {
            // 如果中心点小于中点，放入第一个子节点
            child0_triangles.push_back(triangle);
        } else {
            // 否则放入第二个子节点
            child1_triangles.push_back(triangle);
        }
    }

    // 如果分割后有一个子节点为空，则不进行分割
    if (child0_triangles.empty() || child1_triangles.empty()) {
        return;
    }

    // 创建两个新的子节点
    auto *child0 = new BVHNode{};
    auto *child1 = new BVHNode{};
    // 将子节点链接到父节点
    node->children[0] = child0;
    node->children[1] = child1;
    // 清空当前节点的三角形列表
    node->triangles.clear();
    node->triangles.shrink_to_fit();
    // 将三角形列表移动到子节点
    child0->triangles = std::move(child0_triangles);
    child1->triangles = std::move(child1_triangles);
    // 更新子节点的包围盒
    child0->updateBounds();
    child1->updateBounds();

    // 递归处理两个子节点
    recursiveSplit(child0);
    recursiveSplit(child1);
}

std::optional<HitInfo> BVH::intersect(const Ray &ray, float t_min, float t_max) const {
    // 初始化最近交点信息，初始值为空（表示尚未找到交点）
    std::optional<HitInfo> closest_hit_info;
    // 从根节点开始递归遍历BVH树，寻找与光线在[t_min, t_max]范围内的最近交点
    recursiveIntersect(root, ray, t_min, t_max, closest_hit_info);
    // 返回找到的最近交点信息（如果没有交点则返回空）
    return closest_hit_info;
}

void BVH::recursiveIntersect(BVHNode* node, const Ray& ray, float t_min, float t_max, std::optional<HitInfo>& closest_hit_info) const {
    // 检查当前节点的包围盒是否与光线相交
    // 如果不相交，则无需继续检查该节点及其子节点
    if (!node->bounds.hasIntersection(ray, t_min, t_max)) {
        return;
    }

    // 判断当前节点是否为叶子节点（triangles为空表示是非叶子节点）
    if (node->triangles.empty()) {
        // 递归检查子节点
        recursiveIntersect(node->children[0], ray, t_min, t_max, closest_hit_info);
        recursiveIntersect(node->children[1], ray, t_min, t_max, closest_hit_info);
    } else {
        // 当前节点是叶子节点，遍历其中的所有三角形
        for (const auto& triangle : node->triangles) {
            // 计算光线与当前三角形的交点信息
            auto hit_info = triangle.intersect(ray, t_min, t_max);
            // 如果存在交点
            if (hit_info) {
                // 更新t_max为当前交点的距离，用于后续的剪枝优化 这样可以避免检查比当前最近交点更远的物体
                t_max = hit_info->t;
                // 更新最近交点信息
                closest_hit_info = hit_info;
            }
        }
    }
}
