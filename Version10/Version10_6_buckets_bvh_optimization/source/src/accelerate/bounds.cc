#include "accelerate/bounds.hpp"

bool Bounds::hasIntersection(const Ray &ray, float t_min, float t_max) const {
    /* 
    t_min 和 t_max 的核心作用
        t_min 和 t_max 用来限定我们关心的光线范围，只检测这个范围内的相交，忽略范围外的相交，目的是：
        过滤掉无效的相交（比如光线起点后方的相交）；
        优化性能（只检测「有意义的区间」，避免多余计算）；
        处理嵌套 / 遮挡场景（比如已经找到一个更近的交点，就把 t_max 设为这个交点的 t，后续只检测更近的物体）。
    */
    // 计算交点参数：t1 和 t2 分别是光线与包围盒六个面的交点参数，分别对应 b_min 和 b_max 三个轴的交点
    glm::vec3 t1 = (b_min - ray.origin) / ray.direction;  
    glm::vec3 t2 = (b_max - ray.origin) / ray.direction; 

    // 统一方向：通过 glm::min 和 glm::max 得到每个轴的进入 tmin 和离开 tmax 参数，消除光线方向正负的影响
    glm::vec3 tmin = glm::min(t1, t2);   // 所有轴进入时间的最小值
    glm::vec3 tmax = glm::max(t1, t2);   // 所有轴离开时间的最大值

    // 求交集区间：near：所有轴进入时间的最大值（光线真正进入包围盒的时间） far：所有轴离开时间的最小值（光线真正离开包围盒的时间）
    // 最外层max和min是为了确保 near/far 落在 [t_min, t_max] 区间内
    float near = glm::max(t_min, glm::max(tmin.x, glm::max(tmin.y, tmin.z)));
    float far = glm::min(t_max, glm::min(tmax.x, glm::min(tmax.y, tmax.z)));
 
    if(near <= t_min && far >= t_max){
        return false;
    }

    return glm::max(near, t_min) <= glm::min(far, t_max); 
}



bool Bounds::hasIntersection(const Ray &ray, const glm::vec3 &inv_direction, float t_min, float t_max) const {
    // 计算交点参数：t1 和 t2 分别是光线与包围盒六个面的交点参数，分别对应 b_min 和 b_max 三个轴的交点
    glm::vec3 t1 = (b_min - ray.origin) * inv_direction;  
    glm::vec3 t2 = (b_max - ray.origin) * inv_direction; 

    // 统一方向：通过 glm::min 和 glm::max 得到每个轴的进入 tmin 和离开 tmax 参数，消除光线方向正负的影响
    glm::vec3 tmin = glm::min(t1, t2);   // 所有轴进入时间的最小值
    glm::vec3 tmax = glm::max(t1, t2);   // 所有轴离开时间的最大值

    // 求交集区间：near：所有轴进入时间的最大值（光线真正进入包围盒的时间） far：所有轴离开时间的最小值（光线真正离开包围盒的时间）
    // 最外层max和min是为了确保 near/far 落在 [t_min, t_max] 区间内
    float near = glm::max(t_min, glm::max(tmin.x, glm::max(tmin.y, tmin.z)));
    float far = glm::min(t_max, glm::min(tmax.x, glm::min(tmax.y, tmax.z)));
 
    if(near <= t_min && far >= t_max){
        return false;
    }

    return glm::max(near, t_min) <= glm::min(far, t_max); 
}

