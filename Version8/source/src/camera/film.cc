#include <fstream>
#include "util/rgb.hpp"
#include "camera/film.hpp"

Film::Film(size_t width, size_t height): width(width), height(height){
    pixels.resize(width * height);
}

void Film::save(const std::filesystem::path &filename){ // 需要C++ 17
    std::ofstream file(filename, std::ios::binary);
    file << "P6\n" << width << ' ' << height << "\n255\n";  //P3是用Ascii码，P6是用二进制，颜色的每个分量分到8bit
    
    for (size_t y = 0; y < height; y++){
        for (size_t x = 0; x < width; x++) {
            auto pixel = getPixel(x, y);   // 获取一个像素
            RGB rgb(pixel.color / static_cast<float>(pixel.sample_count));  // 平均颜色
            file << static_cast<uint8_t>(rgb.r) << static_cast<uint8_t>(rgb.g) << static_cast<uint8_t>(rgb.b);
        }    
    }
    // ppm
    // P3
    // 1920 1080
    // 255
    // 255 0 0
    // 0 255 0 
} 
