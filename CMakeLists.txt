cmake_minimum_required(VERSION 3.20)
project(hostctl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/libs/imgui)
set(GLFW_DIR ${CMAKE_SOURCE_DIR}/libs/glfw)
add_subdirectory(${GLFW_DIR})

set(IMGUI_SRC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
)

set(IMGUIFILEDIALOG_SRC
    ${CMAKE_SOURCE_DIR}/libs/ImGuiFileDialog/ImGuiFileDialog.cpp
)

add_executable(hostctl
    src/main.cpp
    src/ThemeManager/themes.cpp
    ${IMGUI_SRC}
    ${IMGUIFILEDIALOG_SRC}
        src/ServiceHelper/serviceHelper.cpp
        src/ServiceHelper/serviceHelper.h
)

target_include_directories(hostctl PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${IMGUI_DIR}/misc/cpp
    
    ${CMAKE_SOURCE_DIR}/libs/ImGuiFileDialog
)

find_package(OpenGL REQUIRED)
target_link_libraries(
    hostctl 
    glfw 
    OpenGL::GL
)
