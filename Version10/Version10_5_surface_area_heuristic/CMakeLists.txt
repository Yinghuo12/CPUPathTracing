cmake_minimum_required(VERSION 3.7)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
enable_language(CXX)

# 设置release模式加速渲染
set(CMAKE_BUILD_TYPE Release)

# Add debugging flags for C++
# set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
# set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# 设置编译选项
project(CPUPathTracing_Version10_5_surface_area_heuristic)

add_subdirectory(source)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/rapidobj)