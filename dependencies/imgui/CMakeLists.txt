cmake_minimum_required(VERSION 3.31)
project(imgui)

add_library(imgui)
target_sources(imgui PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui-master/imgui.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui-master/imgui_demo.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui-master/imgui_draw.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui-master/imgui_tables.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui-master/imgui_widgets.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui-master/backends/imgui_impl_glfw.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui-master/backends/imgui_impl_opengl3.cpp"
)

target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui-master")

target_link_libraries(imgui PUBLIC glfw)