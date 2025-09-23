#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif	

#include <iostream>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

int main() {
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

// window properties 
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // to make macOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // no old OpenGL
    
// window creation    
    GLFWwindow* window;
    window = glfwCreateWindow( 1024, 768, "ImGui loader", NULL, NULL);
    
    if(window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. OpenGL 3.3 support is required!\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
//imgui context creation, flags & setting platform/render backends
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

//main loop
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    do{
        // can cause flickering
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    while( 
        glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0
    );

    std::cout << "goodbye!";
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);    
    glfwTerminate();
    return 0;
}
