#include "util/rgb.hpp"
#include "thread/thread_pool.hpp"
#include "camera/film.hpp"
#include "util/profile.hpp"
#include <fstream>

Film::Film(size_t width, size_t height): width(width), height(height){
    pixels.resize(width * height);
}

void Film::save(const std::filesystem::path &filename){ // 需要C++ 17
    PROFILE("Save to " + filename.string())
    
    std::ofstream file(filename, std::ios::binary);
    file << "P6\n" << width << ' ' << height << "\n255\n";  //P3是用Ascii码，P6是用二进制，颜色的每个分量分到8bit
    
    std::vector<uint8_t> buffer(width * height * 3);  // RGB每个分量占8bit，所以一个像素占3个字节
    thread_pool.parallelFor(width, height, [&](size_t x, size_t y){
        auto pixel = getPixel(x, y);   // 获取一个像素
        RGB rgb(pixel.color / static_cast<float>(pixel.sample_count));  // 平均颜色

        auto idx = (y * width + x) * 3;  // 计算像素在buffer中的位置
        buffer[idx + 0] = rgb.r;
        buffer[idx + 1] = rgb.g;
        buffer[idx + 2] = rgb.b;
    }, false);  // 不是复杂任务，一个线程执行一个任务
    thread_pool.wait();
    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());   

    // ppm
    // P3
    // 1920 1080
    // 255
    // 255 0 0
    // 0 255 0 
} 
