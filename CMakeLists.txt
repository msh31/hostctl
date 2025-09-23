cmake_minimum_required(VERSION 3.20)
project(imgui-loader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/libs/imgui)
set(GLFW_DIR ${CMAKE_SOURCE_DIR}/libs/glfw)
add_subdirectory(libs/glfw)

set(IMGUI_SRC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(imgui-loader
    src/main.cpp
    ${IMGUI_SRC}
)

target_include_directories(imgui-loader PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

find_package(OpenGL REQUIRED)
target_link_libraries(
    imgui-loader 
    glfw 
    OpenGL::GL
)
