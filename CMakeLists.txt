cmake_minimum_required(VERSION 3.10)
project(mygl LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_library(glad STATIC
    external/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    external/glad/include
)

add_executable(mygl
    src/main.cpp
)

target_link_libraries(mygl
    glad
    glfw
    OpenGL::GL
    dl
    glm::glm
)
