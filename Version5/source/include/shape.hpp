#ifndef __SHAPE_HPP__
#define __SHAPE_HPP__

# include "ray.hpp"
#include <optional>

// 形状类，所有的几何体都继承自这个类 
struct Shape{
    // 所有的形状都要实现这个函数，来进行相交测试
    virtual std::optional<HitInfo> intersect(
        const Ray& ray,
        // t_min和t_max是为了避免自相交的情况，或者说是为了限制交点的范围，避免一些数值误差导致的错误交点 
        float t_min = 1e-5, 
        float t_max = std::numeric_limits<float>::infinity()
    ) const = 0; 
};


#endif // __SHAPE_HPP__