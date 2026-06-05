#pragma once

#include <cmath>

struct Complex {
    union {
        float a;
        float c;
    };
    union {
        float b;
        float d;
    };

    Complex(float a, float b) : a(a), b(b) {}
    Complex(float a) : a(a), b(0) {}
};

inline Complex operator+(Complex lhs, Complex rhs) {
    return { lhs.a + rhs.c, lhs.b + rhs.d };
}

inline Complex operator-(Complex lhs, Complex rhs) {
    return { lhs.a - rhs.c, lhs.b - rhs.d };
}

inline Complex operator*(Complex lhs, Complex rhs) {
    return { lhs.a * rhs.c - lhs.b * rhs.d, lhs.a * rhs.d + lhs.b * rhs.c };
}

inline Complex operator*(Complex lhs, float rhs) {
    return { lhs.a * rhs, lhs.b * rhs };
}

inline Complex operator/(Complex lhs, Complex rhs) {
    float denom = 1.f / (rhs.c * rhs.c + rhs.d * rhs.d);
    return {
        (lhs.a * rhs.c + lhs.b * rhs.d) * denom,
        (lhs.b * rhs.c - lhs.a * rhs.d) * denom
    };
}

inline Complex operator/(Complex lhs, float rhs) {
    return { lhs.a / rhs, lhs.b / rhs };
}

inline float norm(Complex rhs) {
    return std::sqrt(rhs.a * rhs.a + rhs.b * rhs.b);
}

inline Complex sqrt(Complex rhs) {
    float rhs_norm = norm(rhs);
    // 提取虚部 b 的符号
    float sign_b = (rhs.b < 0.f) ? -1.f : 1.f;
    return {
        std::sqrt(std::max(0.f, rhs_norm + rhs.a) * 0.5f),
        sign_b * std::sqrt(std::max(0.f, rhs_norm - rhs.a) * 0.5f),
    };
}