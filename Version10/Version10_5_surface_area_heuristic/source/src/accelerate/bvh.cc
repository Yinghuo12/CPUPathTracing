#include "accelerate/bvh.hpp"
#include "util/debug_macro.hpp"
#include <array>
#include <iostream>

void BVH::build(std::vector<Triangle> &&triangles) {
    // 创建BVH的根节点
    auto *root = new BVHTreeNode{};
    // 使用移动语义将三角形数据转移到根节点中，避免不必要的拷贝
    root->triangles = std::move(triangles);
    // 更新根节点的包围盒，使其包含所有三角形
    root->updateBounds();
    root->depth = 1;  // 根节点的深度为1
    BVHState state{}; 
    size_t triangle_count = root->triangles.size();  // 记录初始三角形数量
    // 递归地分割BVH节点，构建层次结构
    recursiveSplit(root, state);

    std::cout << "Total node count: " << state.total_node_count << std::endl;
    std::cout << "Leaf node count: " << state.leaf_node_count << std::endl;
    std::cout << "Triangle count: " << triangle_count << std::endl;
    std::cout << "Mean Leaf Node Triangle Count: " << static_cast<float>(triangle_count) / static_cast<float>(state.leaf_node_count) << std::endl;
    std::cout << "Max Leaf Node Triangle Count: " << state.max_leaf_node_triangle_count << std::endl;

    recursiveFlatten(root);
}

void BVH::recursiveSplit(BVHTreeNode *node, BVHState &state) {
    state.total_node_count ++;
    // 如果当前节点只有一个三角形，则无需继续分割
    if (node->triangles.size() == 1 || node->depth > 32) {  // 深度限制为32,否则也视为叶子节点
        state.addLeafNode(node);
        return;
    }

    // 计算当前节点包围盒的对角线长度，并确定最长的轴
    auto diag = node->bounds.diagonal();
    float min_cost = std::numeric_limits<float>::infinity();
    std::vector<Triangle> child0_triangles, child1_triangles;
    for(size_t axis = 0; axis < 3; axis++){
        for(size_t i = 0; i < 11; i++){
            float mid = node->bounds.b_min[axis] + diag[axis] * (i + 1.f) / 12.f;
            Bounds child0_bounds{}, child1_bounds{};
            std::vector<Triangle> child0_triangles_temp, child1_triangles_temp; 
            for (const auto &triangle : node->triangles) {
                // 计算三角形的中心点坐标
                if ((triangle.p0[axis] + triangle.p1[axis] + triangle.p2[axis]) / 3.f < mid) {
                    child0_bounds.expand(triangle.p0);
                    child0_bounds.expand(triangle.p1);
                    child0_bounds.expand(triangle.p2);
                    child0_triangles_temp.push_back(triangle);
                } else {
                    // 否则放入第二个子节点
                    child1_bounds.expand(triangle.p0);
                    child1_bounds.expand(triangle.p1);
                    child1_bounds.expand(triangle.p2);
                    child1_triangles_temp.push_back(triangle);
                }
            }
            if(child0_triangles_temp.empty() || child1_triangles_temp.empty()){
                continue;
            }
            float cost = child0_bounds.area() * child0_triangles_temp.size() + child1_bounds.area() * child1_triangles_temp.size();
            if(cost < min_cost){
                min_cost = cost;
                child0_triangles = std::move(child0_triangles_temp);
                child1_triangles = std::move(child1_triangles_temp);
                node->split_axis = axis;
            }
        }
    }

    // 如果分割后有一个子节点为空，则不进行分割
    if (child0_triangles.empty() || child1_triangles.empty()) {
        state.addLeafNode(node);
        return;
    }

    // 创建两个新的子节点
    auto *child0 = new BVHTreeNode{};
    auto *child1 = new BVHTreeNode{};
    // 将子节点链接到父节点
    node->children[0] = child0;
    node->children[1] = child1;
    // 清空当前节点的三角形列表
    node->triangles.clear();
    node->triangles.shrink_to_fit();
    // 更新子节点的深度
    child0->depth = node->depth + 1;  
    child1->depth = node->depth + 1;
    // 将三角形列表移动到子节点
    child0->triangles = std::move(child0_triangles);
    child1->triangles = std::move(child1_triangles);
    // 更新子节点的包围盒
    child0->updateBounds();
    child1->updateBounds();

    // 递归处理两个子节点
    recursiveSplit(child0, state);
    recursiveSplit(child1, state);
}

// 递归展平BVH树结构为线性数组
size_t BVH::recursiveFlatten(BVHTreeNode *node){
    // 创建新的BVH节点，包含边界框、三角形数据，子节点索引初始化为0
    BVHNode bvh_node{
        node->bounds
        , 0
        , static_cast<uint16_t>(node->triangles.size())
        , static_cast<uint8_t>(node->depth)
        , static_cast<uint8_t>(node->split_axis)
    };
    // 获取当前节点在nodes数组中的索引
    auto idx = nodes.size();
    // 将新节点添加到nodes数组中
    nodes.push_back(bvh_node);
    // 如果当前节点没有三角形（说明是非叶子节点）
    if(bvh_node.triangle_count == 0){
        // 递归处理左子节点
        recursiveFlatten(node->children[0]);
        // 递归处理右子节点，并将返回的索引作为右子节点的索引
        nodes[idx].child1_index = recursiveFlatten(node->children[1]);
    } else {
        nodes[idx].triangle_index = ordered_triangles.size();
        for (const auto &triangle : node->triangles) {
            ordered_triangles.push_back(triangle);
        }
    }
    // 返回当前节点的索引
    return idx;
}

std::optional<HitInfo> BVH::intersect(const Ray &ray, float t_min, float t_max) const {
    // 初始化最近交点信息，初始值为空（表示尚未找到交点）
    std::optional<HitInfo> closest_hit_info;
    // 从根节点开始递归遍历BVH树，寻找与光线在[t_min, t_max]范围内的最近交点
    // 返回找到的最近交点信息（如果没有交点则返回空）

    // #ifdef WITH_DEBUG_INFO
    // size_t bounds_test_count = 0, triangle_test_count = 0;
    // #endif
    DEBUG_LINE(size_t bounds_test_count = 0, triangle_test_count = 0)

    glm::bvec3 dir_is_neg = {ray.direction.x < 0, ray.direction.y < 0, ray.direction.z < 0};  // 计算每个轴光线分量是否为负
    glm::vec3 inv_direction = 1.f / ray.direction;  // 预计算每个轴光线分量的倒数，优化浮点数除法

    std::array<int, 32> stack;
    auto ptr = stack.begin();
    size_t current_node_index = 0;

    while(true){
        auto &node = nodes[current_node_index];

        // #ifdef WITH_DEBUG_INFO
        // bounds_test_count++;
        // #endif
        DEBUG_LINE(bounds_test_count++)

        // 检查当前节点的包围盒是否与光线相交
        // 如果不相交，则无需继续检查该节点及其子节点
        if (!node.bounds.hasIntersection(ray, inv_direction, t_min, t_max)) {
            if(ptr == stack.begin()) break;
            current_node_index = *(--ptr);  // 栈指针指向的索引是父节点
            continue;
        }

        // 判断当前节点是否为叶子节点（triangles为空表示是非叶子节点）
        if (node.triangle_count == 0) {
            if(dir_is_neg[node.split_axis]){
                // 光线在分割轴上为负方向，优先访问右子节点，将左子节点压栈
                *(ptr++) = current_node_index + 1;
                current_node_index = node.child1_index;
            } else {
                // 光线在分割轴上为正方向，优先访问左子节点，将右子节点压栈
                current_node_index++;
                *(ptr++) = node.child1_index;
            }
        } else{
            auto triangle_iter = ordered_triangles.begin() + node.triangle_index;

            // #ifdef WITH_DEBUG_INFO
            // triangle_test_count += node.triangle_count; 
            // #endif
            DEBUG_LINE(triangle_test_count += node.triangle_count)

            // 当前节点是叶子节点，遍历其中的所有三角形
            for (size_t i = 0; i < node.triangle_count; i++) {
                // 计算光线与当前三角形的交点信息
                auto hit_info = triangle_iter->intersect(ray, t_min, t_max);
                ++triangle_iter;
                // 如果存在交点
                if (hit_info) {
                    // 更新t_max为当前交点的距离，用于后续的剪枝优化 这样可以避免检查比当前最近交点更远的物体
                    t_max = hit_info->t;
                    // 更新最近交点信息
                    closest_hit_info = hit_info;

                    #ifdef WITH_DEBUG_INFO
                    closest_hit_info->bounds_depth = node.depth;
                    #endif
                    DEBUG_LINE(closest_hit_info->bounds_depth = node.depth)
                }
            }
            if(ptr == stack.begin()) break;
            current_node_index = *(--ptr);
        }

        if(closest_hit_info.has_value()){
            DEBUG_LINE(closest_hit_info->bounds_test_count = bounds_test_count)
            DEBUG_LINE(closest_hit_info->triangle_test_count = triangle_test_count)
        }
    }
    return closest_hit_info;
}