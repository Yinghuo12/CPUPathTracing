set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE source CONFIGURE_DEPENDS src/*.cc)    # 源文件  CONFIGURE_DEPENDS 确保新增/删除 .cc 文件后，构建系统能自动更新文件列表并重新编译

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/)
add_executable(CPUPathTracing_Version7 ${source})

target_include_directories(CPUPathTracing_Version7 PUBLIC include)   # 目录文件
target_compile_definitions(CPUPathTracing_Version7 PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_RADIANS GLM_FORCE_LEFT_HANDED)  # 定义宏 透视投影矩阵，lookat函数要满足我们坐标系约定

target_link_libraries(CPUPathTracing_Version7 PRIVATE glm)  # 链接库