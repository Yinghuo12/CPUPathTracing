#ifndef __BASE_RENDERER_HPP__
#define __BASE_RENDERER_HPP__

#include "../camera/camera.hpp"
#include "../shape/scene.hpp"

#define DEFINE_RENDERER(Name) \
    class Name##Renderer: public BaseRenderer { \
    public: \
        Name##Renderer(Camera& camera, const Scene& scene) : BaseRenderer(camera, scene) {} \
    private: \
        glm::vec3 renderPixel(const glm::ivec3 &pixel_coord) override; \
    }; \
    
class BaseRenderer {
public:
    BaseRenderer(Camera& camera, const Scene& scene) : camera(camera), scene(scene) {}
    void render(size_t spp, const std::filesystem::path &filename);
private:
    virtual glm::vec3 renderPixel(const glm::ivec3 &pixel_coord) = 0;  // 改为三维，第三个维度是当前渲染的采样数
protected:
    Camera& camera;
    const Scene& scene;
};


#endif // __BASE_RENDERER_HPP__