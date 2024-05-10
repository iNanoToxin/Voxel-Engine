project(imgui)

file(GLOB SOURCE "./*.hpp")

add_library(imgui-header INTERFACE)
add_library(imgui INTERFACE)

target_include_directories(imgui INTERFACE imgui-header)
