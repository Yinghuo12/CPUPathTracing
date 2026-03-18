#ifndef __SHAPE_HPP__
#define __SHAPE_HPP__

# include "ray.hpp"
#include <optional>

// 形状类，所有的几何体都继承自这个类 
struct Shape{
    // 所有的形状都要实现这个函数，来进行相交测试
    virtual std::optional<HitInfo> intersect(const Ray& ray, float t_min, float t_max) const = 0; 
};


#endif // __SHAPE_HPP__