cmake_minimum_required(VERSION 3.31)
project(OpenGL_Template)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES main.cpp
        header_files/shader.h
        dependencies/stb_image/src/stb_image.cpp)

add_subdirectory("dependencies/glfw-3.4")
add_subdirectory("dependencies/glad")
add_subdirectory("dependencies/glm")
add_subdirectory("dependencies/assimp-master")
add_subdirectory("dependencies/stb_image")
add_subdirectory("dependencies/imgui")

add_executable(OpenGL_Template ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(OpenGL_Template PRIVATE glfw glad glm assimp stb_image imgui)