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

# ---- pybind11 (embedded Python) ----
include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.13.6
)
FetchContent_MakeAvailable(pybind11)

add_executable(mygl
    src/main.cpp
)

target_link_libraries(mygl
    glad
    glfw
    OpenGL::GL
    dl
    glm::glm
    pybind11::embed
)

# Optional: copy python_src next to the built executable so relative paths work
add_custom_command(TARGET mygl POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/assets
          $<TARGET_FILE_DIR:mygl>/assets
)
