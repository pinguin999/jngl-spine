cmake_minimum_required(VERSION 3.6)
set(CMAKE_COLOR_DIAGNOSTICS 1)

project(jngl-spine LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 14)

add_subdirectory("subprojects/jngl")
set(SPINE_SET_COMPILER_FLAGS OFF CACHE BOOL "Set compiler flags" FORCE)
add_subdirectory("subprojects/spine-runtimes/spine-cpp")

file(GLOB SRC src/*.cpp)
add_executable(jngl-spine ${SRC})

target_link_libraries(jngl-spine jngl spine-cpp)
