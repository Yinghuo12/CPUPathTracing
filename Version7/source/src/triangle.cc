#include "triangle.hpp"

std::optional<HitInfo> Triangle::intersect(const Ray& ray, float t_min, float t_max) const {
    // Möller–Trumbore算法
    glm::vec3 e1 = p1 - p0; 
    glm::vec3 e2 = p2 - p0; 
    glm::vec3 s1 = glm::cross(ray.direction, e2); //  辅助向量1：计算光线方向与第二条边向量的叉积
    float inv_det = 1.f / glm::dot(s1, e1);       //  计算行列式的倒数
    
    glm::vec3 s = ray.origin - p0;       //  计算从三角形顶点到光线起点的向量
    float u = glm::dot(s1, s) * inv_det; //  计算第一个重心坐标u并进行范围检查
    if(u < 0 || u > 1){ return {}; }     //  如果u不在0-1范围内，光线与三角形不相交

    glm::vec3 s2 = glm::cross(s, e1);                // 辅助向量2: 计算从三角形顶点到光线起点的向量与第一条边向量的叉积
    float v = glm::dot(s2, ray.direction) * inv_det; //  计算第二个重心坐标v并进行范围检查
    if(v < 0 || u + v > 1){ return {}; }             //  如果v不在0-1范围内或u+v超过1，光线与三角形不相交

    float hit_t = glm::dot(s2, e2) * inv_det;  //  计算光线与三角形的交点距离
    if(hit_t > t_min && hit_t < t_max){        //  检查交点是否在有效范围内
        glm::vec3 hit_point = ray.hit(hit_t);  //  计算交点位置
        glm::vec3 normal = (1.f - u - v) * n0 + u * n1 + v * n2;  //  使用重心坐标插值计算法线
        return HitInfo{hit_t, hit_point, glm::normalize(normal)}; //  返回相交信息，包括距离、交点和法线
    }
    return {}; //  如果没有相交，返回空结果
}