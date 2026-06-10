#ifndef __FILM_HPP__
#define __FILM_HPP__

#include <cstddef>
#include <filesystem>
#include <vector>
#include <glm/glm.hpp>
/* 胶片类：存储了每个像素的颜色, 并且负责把最终的图像存储成一张图片 */

struct Pixel{
    glm::vec3 color{0, 0, 0};   // 每个像素的颜色
    int sample_count {0};       // 记录每个像素有多少个采样点
};

class Film{
public:
    Film(size_t width, size_t height);
    void save(const std::filesystem::path &filename) const; // 需要C++ 17

    size_t getWidth() const { return width; };
    size_t getHeight() const { return height; };
    Pixel getPixel(size_t x, size_t y) const { return pixels[y * width + x]; };
    void addSample(size_t x, size_t y, const glm::vec3 &color){
        // 由于路径追踪中可能会出现NaN值，如果不进行判断直接保存到像素中，最终会导致整张图像变成黑色
        if(glm::any(glm::isnan(color))){
            return;
        }
        pixels[y * width + x].color += color;
        pixels[y * width + x].sample_count++;
    } 

    void clear(){ pixels.clear(); pixels.resize(width * height); } // 清空所有像素的颜色
    void setResolution(size_t width, size_t height) { // 设置分辨率
        this->width = width;
        this->height = height;
        pixels.resize(width * height);
    } 
    std::vector<uint8_t> generateRGBABuffer(); // 生成RGBA格式的图像数据，方便保存成图片

private:
    size_t width, height;
    std::vector<Pixel> pixels;
};

#endif