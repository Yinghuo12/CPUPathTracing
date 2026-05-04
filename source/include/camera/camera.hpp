#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "film.hpp"
#include "ray.hpp"

class Camera {
public:
    Camera(Film &film, const glm::vec3 &pos, const glm::vec3 &viewpoint, float fovy);  // 胶片 相机位置 看向的点 在垂直方向上的视野角度 

    // 生成光线,即返回世界空间下的一根光线
    Ray generateRay(const glm::ivec2 &pixel_coord, const glm::vec2 &offset = { 0.5, 0.5 }) const; // 屏幕下的像素坐标和像素偏移

    Film &getFilm() { return film; }
    const Film &getFilm() const { return film; }


private:
    Film &film;
    glm::vec3 pos;   // 相机位置(世界空间)

    glm::mat4 camera_from_clip;  // 从裁剪空间到相机空间
    glm::mat4 world_from_camera; // 从相机空间到世界空间
};

#endif // __CAMERA__HPP__