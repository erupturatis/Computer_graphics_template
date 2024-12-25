cmake_minimum_required(VERSION 3.29)
project(computer_graphics)
set(CMAKE_CXX_STANDARD 20)

# Add the executable
add_executable(computer_graphics
        main.cpp
        Camera.cpp
        Mesh.cpp
        Model3D.cpp
        Shader.cpp
        stb_image.cpp
        tiny_obj_loader.cpp
        BoundingBoxes.cpp
        pure_functions.cpp
        CameraAnimation.cpp
        Window.cpp
        Mouse.cpp
        GlobalConfigs.cpp
        GlobalObjects.cpp
        Bindings.cpp
        Bindings.h
        Callbacks.cpp
        Callbacks.h
        Scene.cpp
        Scene.h
)

target_include_directories(computer_graphics PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs)
target_compile_definitions(computer_graphics PRIVATE GLEW_STATIC)

target_link_libraries(computer_graphics PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_libraries/glew32s.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_libraries/libglfw3dll.a
        opengl32
)

