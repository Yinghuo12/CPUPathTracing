#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "film.hpp"
#include "ray.hpp"

enum class Direction {
    Forward, 
    Backward, 
    Left, 
    Right, 
    Up, 
    Down
};

class Camera {
public:
    Camera(Film &film, const glm::vec3 &pos, const glm::vec3 &viewpoint, float fovy);  // 胶片 相机位置 看向的点 在垂直方向上的视野角度 

    // 生成光线,即返回世界空间下的一根光线
    Ray generateRay(const glm::ivec2 &pixel_coord, const glm::vec2 &offset = { 0.5, 0.5 }) const; // 屏幕下的像素坐标和像素偏移

    Film &getFilm() { return film; }
    const Film &getFilm() const { return film; }
    
    void move(float dt, Direction direction); // 根据输入的方向和时间增量移动相机
    void turn(const glm::vec2 &delta);        // 根据输入的鼠标移动增量转动相机
    void zoom(float delta);                   // 根据输入的鼠标滚轮增量缩放相机

    void print();
    
private:
    void update();

private:
    Film &film;
    glm::vec3 pos;                   // 相机位置(世界空间)

    float fovy;                     // 在垂直方向上的视野角度
    glm::vec3 view_direction;         // 相机朝向(世界空间)
    float theta, phi;                // 球坐标系下的相机位置
    float move_speed = 2;            // 相机移动速度
    glm::vec2 turn_speed {0.15, 0.07};            // 相机转动速度

    glm::mat4 camera_from_clip;  // 从裁剪空间到相机空间
    glm::mat4 world_from_camera; // 从相机空间到世界空间
};

#endif // __CAMERA__HPP__