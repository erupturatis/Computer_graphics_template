cmake_minimum_required(VERSION 3.29)
project(computer_graphics)
set(CMAKE_CXX_STANDARD 20)

# Add the executable
add_executable(computer_graphics main.cpp)

target_include_directories(computer_graphics PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Link GLFW (using dynamic libraries)
target_link_libraries(computer_graphics PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_libraries/glfw3.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_libraries/libglfw3dll.a
    ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_libraries/glew32.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_libraries/glew32.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_libraries/glew32s.lib
    opengl32  # Link the OpenGL library on Windows
)

