#include "accelerate/scene_bvh.hpp"
#include "util/debug_macro.hpp"
#include <array>
#include <iostream>

void SceneBVH::build(std::vector<ShapeInstance> &&instances) {
    // 创建SceneBVH的根节点
    auto *root = allocator.allocate();
     
    // 使用移动语义将三角形数据转移到根节点中，避免不必要的拷贝
    auto temp_instances = std::move(instances);
    for(auto &instance: temp_instances){
        if(instance.shape.getBounds().isValid()){
            instance.updateBounds(); // 更新世界空间下的包围盒
            root->instances.push_back(instance);
        } else {
            infinity_instances.push_back(instance);
        }
    }

    // 更新根节点的包围盒，使其包含所有三角形
    root->updateBounds();
    root->depth = 1;  // 根节点的深度为1
    SceneBVHState state{}; 
    size_t instance_count = root->instances.size();  // 记录初始三角形数量
    // 递归地分割SceneBVH节点，构建层次结构
    recursiveSplit(root, state);

    std::cout << "Total node count: " << state.total_node_count << std::endl;
    std::cout << "Leaf node count: " << state.leaf_node_count << std::endl;
    std::cout << "ShapeInstance count: " << instance_count << std::endl;
    std::cout << "Mean Leaf Node ShapeInstance Count: " << static_cast<float>(instance_count) / static_cast<float>(state.leaf_node_count) << std::endl;
    std::cout << "Max Leaf Node ShapeInstance Count: " << state.max_leaf_node_instance_count << std::endl;
    std::cout << "Max Leaf Node Depth: " << state.max_leaf_node_depth << std::endl;

    nodes.reserve(state.total_node_count); // 预先分配内存，避免多次重新分配
    ordered_instances.reserve(instance_count); // 预先分配内存，避免多次重新分配
    
    recursiveFlatten(root);
}

void SceneBVH::recursiveSplit(SceneBVHTreeNode *node, SceneBVHState &state) {
    state.total_node_count ++;
    // 如果当前节点只有一个三角形，则无需继续分割
    if (node->instances.size() == 1 || node->depth > 32) {  // 深度限制为32,否则也视为叶子节点
        state.addLeafNode(node);
        return;
    }

    // 计算当前节点包围盒的对角线长度，并确定最长的轴
    auto diag = node->bounds.diagonal();
    float min_cost = std::numeric_limits<float>::infinity();
    size_t min_split_index = 0;  // 记录最优分割位置
    Bounds min_child0_bounds{}, min_child1_bounds{};
    size_t min_child0_instance_count = 0, min_child1_instance_count = 0; // 记录最优分割后的左右子节点三角形数量
    constexpr size_t bucket_count = 12;   // 桶的数量
    std::vector<size_t> instance_indices_buckets[3][bucket_count] = {};  // 用于存储每个轴的每个桶中的三角形索引
    for(size_t axis = 0; axis < 3; axis++){
        Bounds bounds_buckets[bucket_count] = {};
        size_t instance_count_buckets[bucket_count] = {};
        size_t instance_idx = 0;  // 记录当前处理的三角形索引
        for (const auto &instance : node->instances) {
            size_t bucket_idx = glm::clamp<size_t>(
                glm::floor((instance.center[axis] - node->bounds.b_min[axis]) * bucket_count / diag[axis]), 0, bucket_count - 1
            );
            
            bounds_buckets[bucket_idx].expand(instance.bounds);
            instance_count_buckets[bucket_idx]++;
            instance_indices_buckets[axis][bucket_idx].push_back(instance_idx);
            instance_idx++;
        }

        Bounds left_bounds = bounds_buckets[0];
        size_t left_instance_count = instance_count_buckets[0];

        for (size_t i = 1; i <= bucket_count - 1; i++) {
            Bounds right_bounds{};
            size_t right_instance_count = 0;
            for (size_t j = bucket_count - 1; j >= i; j--) {
                right_bounds.expand(bounds_buckets[j]);
                right_instance_count += instance_count_buckets[j];
            }

            if (right_instance_count == 0) {
                break;
            }
            if (left_instance_count != 0) {
                float cost = left_instance_count * left_bounds.area() + right_instance_count * right_bounds.area();
                if (cost < min_cost) {
                    min_cost = cost;
                    node->split_axis = axis;
                    min_split_index = i;
                    min_child0_bounds = left_bounds;
                    min_child1_bounds = right_bounds;
                    // 优化：记录左右子节点三角形数量，在push_back之前可以预留空间
                    min_child0_instance_count = left_instance_count;
                    min_child1_instance_count = right_instance_count;
                }
            }

            left_bounds.expand(bounds_buckets[i]);
            left_instance_count += instance_count_buckets[i];
        }
    }

    if(min_split_index == 0){
        // 如果分割位置为0，则将所有三角形分配给第一个子节点
        state.addLeafNode(node);
        return;
    } 

    // 创建两个新的子节点
    auto *child0 = allocator.allocate();
    auto *child1 = allocator.allocate();
    // 将子节点链接到父节点
    node->children[0] = child0;
    node->children[1] = child1;

    // 根据最优分割位置将三角形分配到两个子节点中
    // 优化：预留空间，避免多次重新分配
    child0->instances.reserve(min_child0_instance_count);
    child1->instances.reserve(min_child1_instance_count);
    for(size_t i = 0; i < min_split_index; i++){
        for(size_t idx : instance_indices_buckets[node->split_axis][i]){
            child0->instances.push_back(node->instances[idx]);
        }
    }
    for(size_t i = min_split_index; i < bucket_count; i++){
        for(size_t idx : instance_indices_buckets[node->split_axis][i]){
            child1->instances.push_back(node->instances[idx]);
        }
    }

    // 清空当前节点的三角形列表
    node->instances.clear();
    node->instances.shrink_to_fit();

    // 更新子节点的深度
    child0->depth = node->depth + 1;  
    child1->depth = node->depth + 1;

    // 更新子节点的包围盒
    // child0->updateBounds();
    // child1->updateBounds();
    // 优化
    child0->bounds = min_child0_bounds;
    child1->bounds = min_child1_bounds;

    // 递归处理两个子节点
    recursiveSplit(child0, state);
    recursiveSplit(child1, state);
}

// 递归展平SceneBVH树结构为线性数组
size_t SceneBVH::recursiveFlatten(SceneBVHTreeNode *node){
    // 创建新的SceneBVH节点，包含边界框、三角形数据，子节点索引初始化为0
    SceneBVHNode bvh_node{
        node->bounds
        , 0
        , static_cast<uint16_t>(node->instances.size())
        , static_cast<uint8_t>(node->split_axis)
    };
    // 获取当前节点在nodes数组中的索引
    auto idx = nodes.size();
    // 将新节点添加到nodes数组中
    nodes.push_back(bvh_node);
    // 如果当前节点没有三角形（说明是非叶子节点）
    if(bvh_node.instance_count == 0){
        // 递归处理左子节点
        recursiveFlatten(node->children[0]);
        // 递归处理右子节点，并将返回的索引作为右子节点的索引
        nodes[idx].child1_index = recursiveFlatten(node->children[1]);
    } else {
        nodes[idx].instance_index = ordered_instances.size();
        for (const auto &instance : node->instances) {
            ordered_instances.push_back(instance);
        }
    }
    // 返回当前节点的索引
    return idx;
}

std::optional<HitInfo> SceneBVH::intersect(const Ray &ray, float t_min, float t_max) const {
    // 初始化最近交点信息，初始值为空（表示尚未找到交点）
    std::optional<HitInfo> closest_hit_info;
    const ShapeInstance * closest_instance = nullptr;  // 记录最近交点的实例

    // 从根节点开始递归遍历SceneBVH树，寻找与光线在[t_min, t_max]范围内的最近交点
    // 返回找到的最近交点信息（如果没有交点则返回空）
    DEBUG_LINE(size_t bounds_test_count = 0)

    glm::bvec3 dir_is_neg = {ray.direction.x < 0, ray.direction.y < 0, ray.direction.z < 0};  // 计算每个轴光线分量是否为负
    glm::vec3 inv_direction = 1.f / ray.direction;  // 预计算每个轴光线分量的倒数，优化浮点数除法

    std::array<int, 32> stack;
    auto ptr = stack.begin();
    size_t current_node_index = 0;

    while(true){
        auto &node = nodes[current_node_index];
        DEBUG_LINE(bounds_test_count++)

        // 检查当前节点的包围盒是否与光线相交
        // 如果不相交，则无需继续检查该节点及其子节点
        if (!node.bounds.hasIntersection(ray, inv_direction, t_min, t_max)) {
            if(ptr == stack.begin()) break;
            current_node_index = *(--ptr);  // 栈指针指向的索引是父节点
            continue;
        }

        // 判断当前节点是否为叶子节点（instances为空表示是非叶子节点）
        if (node.instance_count == 0) {
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
            auto instance_iter = ordered_instances.begin() + node.instance_index;

            // 当前节点是叶子节点，遍历其中的所有三角形
            for (size_t i = 0; i < node.instance_count; i++) {
                // 计算光线与当前三角形的交点信息
                auto ray_object = ray.objectFromWorld(instance_iter->object_from_world);    // 把Ray转换到物体坐标系
                auto hit_info = instance_iter->shape.intersect(ray_object, t_min, t_max);
                DEBUG_LINE(ray.bounds_test_count += ray_object.bounds_test_count)
                DEBUG_LINE(ray.triangle_test_count += ray_object.triangle_test_count)
                // 如果存在交点
                if (hit_info) {
                    t_max = hit_info->t;
                    closest_hit_info = hit_info;
                    closest_instance = &(*instance_iter);
                }
                ++instance_iter;
            }
            if(ptr == stack.begin()) break;
            current_node_index = *(--ptr);
        }
    }

    // 检查是否有无限距离的实例
    for(const auto &infinity_instance: infinity_instances){
        Ray ray_object = ray.objectFromWorld(infinity_instance.object_from_world);
        std::optional<HitInfo> hit_info = infinity_instance.shape.intersect(ray_object, t_min, t_max);

        DEBUG_LINE(ray.bounds_test_count += ray_object.bounds_test_count)
        DEBUG_LINE(ray.triangle_test_count += ray_object.triangle_test_count)

        if (hit_info) {
            t_max = hit_info->t;
            closest_hit_info = hit_info;
            closest_instance = &infinity_instance;
        }
    } 

    if(closest_instance){
        // 把交点信息转换回世界坐标系
        closest_hit_info->hit_point = closest_instance->world_from_object * glm::vec4(closest_hit_info->hit_point, 1.f);
        closest_hit_info->normal = glm::normalize(glm::vec3(glm::transpose(glm::inverse(closest_instance->world_from_object)) * glm::vec4(closest_hit_info->normal, 0.f)));
        closest_hit_info->material = closest_instance->material;  // 记录最近实例材质
    }

    DEBUG_LINE(ray.bounds_test_count += bounds_test_count)
    return closest_hit_info;
}