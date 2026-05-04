#ifndef __RGB_HPP__
#define __RGB_HPP__

#include <glm/glm.hpp>
#include <array>

class RGB Lerp(const RGB &a, const RGB &b, float t);  // 线性插值

class RGB{
public:
    inline static RGB GenerateHeatmapRGB(float t){
        std::array<RGB, 25> color_pallet{
            // 调色盘
            RGB { 68, 1, 84  },
            RGB { 71, 17, 100 },
            RGB { 72, 31, 112 },
            RGB { 71, 45, 123 },
            RGB { 68, 58, 131 },

            RGB { 64, 70, 136 },
            RGB { 59, 82, 139 },
            RGB { 54, 93, 141 },
            RGB { 49, 104, 142 },
            RGB { 44, 114, 142 },

            RGB { 40, 124, 142 },
            RGB { 36, 134, 142 },
            RGB { 33, 144, 140 },
            RGB { 31, 154, 138 },
            RGB { 32, 164, 134 },

            RGB { 39, 173, 129 },
            RGB { 53, 183, 121 },
            RGB { 71, 193, 110 },
            RGB { 93, 200, 99  },
            RGB { 117, 208, 84 },

            RGB { 143, 215, 68  },
            RGB { 170, 220, 50  },
            RGB { 199, 224, 32  },
            RGB { 227, 228, 24  },
            RGB { 253, 231, 37  },
        };

        if(t < 0 || t >= 1){
            return RGB{ 255, 0, 0};  // 错误颜色: 红色
        } 
        // 线性插值
        float idx_float = t * color_pallet.size();
        size_t idx = glm::floor(idx_float);
        return Lerp(color_pallet[idx], color_pallet[idx + 1], glm::fract(idx_float));  // fract: 求小数部分

    }
    RGB(int r, int g, int b): r(r), g(g), b(b){}

    // 将物理意义的光照强度转换为RGB类
    RGB(const glm::vec3 &color){
        r = glm::clamp<int>(glm::pow(color.x, 1.0 / 2.2) * 255, 0, 255);
        g = glm::clamp<int>(glm::pow(color.y, 1.0 / 2.2) * 255, 0, 255);
        b = glm::clamp<int>(glm::pow(color.z, 1.0 / 2.2) * 255, 0, 255);
    }

    // 隐式转换函数，将RGB转换为glm::vec3  (将RGB颜色转换为物理意义上的光照强度)
    operator glm::vec3() const{
        return glm::vec3(
            glm::pow(r / 255.f, 2.2),
            glm::pow(g / 255.f, 2.2),
            glm::pow(b / 255.f, 2.2)
        );
    }

public:
    int r, g, b;
};

inline RGB Lerp(const RGB &a, const RGB &b, float t){
    return RGB(
        glm::clamp<int>(a.r + (b.r - a.r) * t, 0, 255),
        glm::clamp<int>(a.g + (b.g - a.g) * t, 0, 255),
        glm::clamp<int>(a.b + (b.b - a.b) * t, 0, 255)
    );
}

#endif // __RGB_HPP__