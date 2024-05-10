cmake_minimum_required(VERSION 3.27)
project(VoxelEngine)

add_subdirectory(third-party/glfw-3.4)
add_subdirectory(third-party/glm)
add_subdirectory(third-party/imgui-1.90.6)

add_executable("${CMAKE_PROJECT_NAME}" main.cpp)

target_link_libraries(VoxelEngine PRIVATE glfw glm imgui)
