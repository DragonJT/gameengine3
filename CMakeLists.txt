cmake_minimum_required(VERSION 3.10)
project(mygl LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

add_subdirectory(external/glfw)
add_subdirectory(external/glm)

add_executable(mygl src/main.cpp)

target_link_libraries(mygl PRIVATE
  glad
  glfw
  glm::glm
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(OpenGL REQUIRED)
    target_link_libraries(mygl PRIVATE OpenGL::OpenGL)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(mygl PRIVATE dl)
endif()

add_custom_command(TARGET mygl POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/assets
          $<TARGET_FILE_DIR:mygl>/assets
)
