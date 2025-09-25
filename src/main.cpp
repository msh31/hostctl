#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif	

#include <iostream>
#include <vector>
#include <string>

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include "fonts/rubik.h"
#include "ThemeManager/themes.h"

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
    window = glfwCreateWindow(525, 300, "DLL Injector", NULL, NULL);
    
    if(window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. OpenGL 3.3 support is required!\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
//imgui context creation, theme/window style, flags & setting platform/render backends
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ThemeManager::ApplyTheme(ThemeType::Dark);
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     
   	
	ImFontConfig font_config;
	font_config.FontDataOwnedByAtlas = false;
	io.Fonts->AddFontFromMemoryTTF((void*)Rubik, Rubik_len, 16.0f, &font_config, NULL);

    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

// TODO: move this
    struct Entry {
        std::string Name;
        bool IsSelected = false;
    };

    std::vector<Entry> mockupEntries = {
        {"test"},
        {"test2"},
        {"test3"}
    };

//main loop
    do{
        // can cause flickering
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
		
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

// main imgui window
        ImGui::Begin("Main Window", nullptr, window_flags);

        ImGui::Text("Process list");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::BeginListBox("##hidden")) {
            for (auto& item : mockupEntries) {
                if (ImGui::Selectable(item.Name.c_str(), item.IsSelected)) {
                    for (auto& other : mockupEntries) {
                        other.IsSelected = false;
                    }
                    item.IsSelected = true;
                }
            }
            ImGui::EndListBox();
        }

        ImGui::End();
		
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
