#ifndef __PROFILE_HPP__
#define __PROFILE_HPP__

#pragma once

#include <chrono>
#include <string>

// 定义一个宏，记录函数执行时间
#define PROFILE(name) Profile __profile(name); 

struct Profile {
    Profile(const std::string &name);
    ~Profile();

    std::string name;   // 名称
    std::chrono::high_resolution_clock::time_point start;  // 记录开始时间
};

#endif // __PROFILE_HPP__