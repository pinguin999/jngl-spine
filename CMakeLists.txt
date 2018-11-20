cmake_minimum_required(VERSION 3.6)

project(jngl-spine LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 14)

add_subdirectory("subprojects/jngl")
# add_subdirectory("subprojects/spine-c/spine-c/include")

file(GLOB SRC src/*.cpp)
add_executable(jngl-spine ${SRC})

target_include_directories(jngl-spine PRIVATE subprojects/spine-c/spine-c/include/)

target_link_libraries(jngl-spine jngl)
