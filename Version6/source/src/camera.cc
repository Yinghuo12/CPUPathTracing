#include "camera.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(Film& film, const glm::vec3 & pos, const glm::vec3& viewpoint, float fovy)
    :film(film), pos(pos) {
    camera_from_clip = glm::inverse(glm::perspective( // 透视投影逆矩阵
        glm::radians(fovy),
        static_cast<float>(film.getWidth()) / static_cast<float>(film.getHeight()),
        1.f, 2.f
    ));
    world_from_camera = glm::inverse(glm::lookAt(pos, viewpoint, {0, 1, 0}));
}

Ray Camera::generateRay(const glm::ivec2& pixel_coord, const glm::vec2& offset) const{
    glm::vec2 ndc = (glm::vec2(pixel_coord) + offset) / glm::vec2(film.getWidth(), film.getHeight());  // 根据屏幕下的像素位置计算ndc下的xy坐标
    ndc.y = 1.f - ndc.y;   // ndc空间的y是向上的，屏幕空间的y是向下的，因此需要反转
    // [0,1]->[-1,1]
    ndc = 2.f * ndc - 1.f; 
    glm::vec4 clip {ndc, 0 , 1}; // CLIP(x,y,0,near）对应 NDC(x/near, y/near, 0)  我们的near是1时 NDC[-1,1] 对应 CLIP[-1,1,0,1] // 裁剪空间下点的坐标
    glm::vec3 world = world_from_camera * camera_from_clip * clip;  // 转换为世界坐标
    return Ray{
        pos,                        // 起点：相机位置
        glm::normalize(world - pos) // 光线方向：相机指向屏幕上像素点对应的世界空间上的点
    };
}
