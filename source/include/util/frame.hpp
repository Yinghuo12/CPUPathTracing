#ifndef __FRAME_HPP__
#define __FRAME_HPP__

#include <glm/glm.hpp>

// 用于计算光线局部反射空间的类 因为只有方向没有位置，所以只能转换方向
class Frame{
public:
    Frame(const glm::vec3 & normal);  // 构造函数，传入法线向量，计算局部坐标轴

    glm::vec3 localFromWorld(const glm::vec3 &direction_world) const;  // 将世界空间中的向量转换到局部空间
    glm::vec3 worldFromLocal(const glm::vec3 &direction_local) const;  // 将局部空间中的向量转换到世界空间
private:
    glm::vec3 x_axis, y_axis, z_axis;  // 局部坐标轴在世界空间中的方向
};


#endif // __FRAME_HPP__