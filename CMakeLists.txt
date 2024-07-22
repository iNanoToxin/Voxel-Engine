cmake_minimum_required(VERSION 3.27)
project(VoxelEngine)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(third-party/glfw-3.4)
add_subdirectory(third-party/glm)
add_subdirectory(third-party/imgui-1.90.6)
add_subdirectory(third-party/glad)
add_subdirectory(third-party/fmt-10.2.1)
add_subdirectory(third-party/stb_image)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${CMAKE_PROJECT_NAME} main.cpp ${SOURCES}
    src/utilities/json.h
    src/rendering/grid_overlay.cpp
    src/rendering/grid_overlay.h)

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data/")
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC SHADERS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/shaders/")
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC TEXTURES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/textures/")

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "src")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw glm glad imgui fmt stb_image)
