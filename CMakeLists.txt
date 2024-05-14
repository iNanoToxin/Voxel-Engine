cmake_minimum_required(VERSION 3.27)
project(VoxelEngine)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(third-party/glfw-3.4)
add_subdirectory(third-party/glm)
add_subdirectory(third-party/imgui-1.90.6)
add_subdirectory(third-party/glad)
add_subdirectory(third-party/fmt-10.2.1)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${CMAKE_PROJECT_NAME} main.cpp ${SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "src")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw glm glad imgui fmt)
