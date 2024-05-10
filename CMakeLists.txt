cmake_minimum_required(VERSION 3.27)
project(VoxelEngine)

add_subdirectory(third-party/glfw-3.4)
add_subdirectory(third-party/glm)

add_executable("${CMAKE_PROJECT_NAME}" main.cpp)

target_link_libraries(VoxelEngine PRIVATE glfw glm)
