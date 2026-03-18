#ifndef __FILM_HPP__
#define __FILM_HPP__

#include <cstddef>
#include <filesystem>
#include <vector>
#include <glm/glm.hpp>
/* 胶片类：存储了每个像素的颜色, 并且负责把最终的图像存储成一张图片 */
class Film{
public:
    Film(size_t width, size_t height);
    void save(const std::filesystem::path &filename); // 需要C++ 17,cmake要设置

    size_t getWidth() const { return width; };
    size_t getHeight() const { return height; };
    glm::vec3 getPixel(size_t x, size_t y) const { return pixels[y * width + x]; };
    void setPixel(size_t x, size_t y, const glm::vec3 &color) {
        pixels[y * width + x] = color; 
    };

private:
    size_t width, height;
    std::vector<glm::vec3> pixels;
};

#endif