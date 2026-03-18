set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/)

add_executable(CPUPathTracing_Version1 main.cc)

target_link_libraries(CPUPathTracing_Version1 glm)
