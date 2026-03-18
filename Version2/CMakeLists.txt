cmake_minimum_required(VERSION 3.7)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
enable_language(CXX)

project(CPUPathTracing_Version2)

add_subdirectory(source)
add_subdirectory(thirdparty/glm)