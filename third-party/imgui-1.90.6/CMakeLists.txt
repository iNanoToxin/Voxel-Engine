project(imgui)

file(GLOB SOURCES "./*.cpp")
file(GLOB HEADERS "./*.h")

add_library(imgui)
target_sources(imgui PRIVATE ${SOURCES})
target_include_directories(imgui PUBLIC "./")
