project(imgui)

file(GLOB SOURCES "./*.cpp" "./backends/*.cpp")
file(GLOB HEADERS "./*.h" "./backends/*.h")

#add_library(imgui-header INTERFACE)
#add_library(imgui INTERFACE)

add_library(imgui)
target_sources(imgui PRIVATE ${SOURCES})
target_include_directories(imgui PUBLIC ${HEADERS})
