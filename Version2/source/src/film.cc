#include <fstream>
#include "film.hpp"

Film::Film(size_t width, size_t height): width(width), height(height){
    pixels.resize(width * height);
}

void Film::save(const std::filesystem::path &filename){ // 需要C++ 17
    std::ofstream file(filename);
    file << "P3\n" << width << ' ' << height << "\n255\n";  //P3是用Ascii码，P6是用二进制，颜色的每个分量分到8bit
    
    for (size_t y = 0; y < height; y++){
        for (size_t x = 0; x < width; x++) {
            const glm::vec3 &color = getPixel(x, y);
            glm::ivec3 color_i = glm::clamp(color * 255.f, 0.f, 255.f);
            file << color_i.x << ' ' << color_i.y << ' ' << color_i.z << '\n';
        }    
    }
    // ppm
    // P3  图片由3通道RGB组成
    // 1920 1080  宽高
    // 255   每个分量最大值
    // 255 0 0
    // 0 255 0 
} 
