cmake_minimum_required(VERSION 3.7)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
enable_language(CXX)

# 设置release模式加速渲染
set(CMAKE_BUILD_TYPE Release)

# 设置编译选项
project(CPUPathTracing_Version6)

add_subdirectory(source)
add_subdirectory(thirdparty/glm)