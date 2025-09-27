#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif	

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include "ImGuiFileDialog.h"

#include "fonts/rubik.h"
#include "ThemeManager/themes.h"

std::string extractProjectName(const std::string& path) {
    std::string temp = path;

    if (!temp.empty() && temp.back() == '\\') {
        temp.pop_back();
    }

    size_t pos = temp.find_last_of('\\');

    if (pos != std::string::npos) {
        return temp.substr(pos + 1);
    } else {
        return temp;
    }
}

bool webServerFound() {
    bool webServerFound = false;
    
    if(fs::exists("C:\\zzz")) {
        webServerFound = true;
    }

    if(fs::exists("C:\\xxx")) {
        webServerFound = true;
    }

    return webServerFound;
}

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
    window = glfwCreateWindow(750, 550, "HostCTL - A simple local network manager for XAMPP/WAMP", NULL, NULL);
    
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
    ImFont* titleFont = io.Fonts->AddFontFromMemoryTTF((void*)Rubik, Rubik_len, 24.0f, &font_config, NULL);

    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

// TODO: move this
std::string projectFolderDirectory, projectName;

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

        float panelWidth = ImGui::GetContentRegionAvail().x * 0.8f;
        float panelHeight = ImGui::GetContentRegionAvail().y * 0.7f;

        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - panelWidth) * 0.5f);
        ImGui::SetCursorPosY((ImGui::GetContentRegionAvail().y - panelHeight) * 0.5f);

        ImGui::BeginChild("HostSetupPanel", ImVec2(panelWidth, panelHeight), true, ImGuiWindowFlags_AlwaysUseWindowPadding);

        ImGui::PushFont(titleFont);
        ImGui::Text("HostCTL");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 15));

        float labelWidth = ImGui::CalcTextSize("Local Domain Name:").x;

        ImGui::Text("Local Domain Name:");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (150 - ImGui::CalcTextSize("Local Domain Name:").x));
        ImGui::SetNextItemWidth(220);
        ImGui::InputText("##domainInput", &projectName);
        ImGui::Dummy(ImVec2(0, 10));

        ImGui::Text("Folder Location:");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (150 - ImGui::CalcTextSize("Folder Location:").x));
        ImGui::SetNextItemWidth(220);
        ImGui::InputText("##folderInput", &projectFolderDirectory);
        ImGui::SameLine();
        if (ImGui::Button("Browse")) {
            IGFD::FileDialogConfig config;
	        config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose Directory", nullptr, config);
        }
        ImGui::Dummy(ImVec2(0, 20));

        if (ImGui::Button("Create Host", ImVec2(120, 0))) { 
            
        }
        ImGui::SameLine();
        if (ImGui::Button("Restart Web Server", ImVec2(150, 0))) {
            
        }
        
        ImGui::Dummy(ImVec2(0, 15)); 
        float statusTextWidth = ImGui::CalcTextSize("Web Server Status: Not Found").x;

        ImGui::TextColored(webServerFound() ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 
                           "Web Server Status: %s", webServerFound() ? "Running" : "Not Found");

        ImGui::EndChild();
        if (ImGuiFileDialog::Instance()->Display("ChooseDirDlgKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                projectFolderDirectory = filePath;
                projectName = extractProjectName(filePath);
            }
            ImGuiFileDialog::Instance()->Close();
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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);    
    glfwTerminate();
    return 0;
}