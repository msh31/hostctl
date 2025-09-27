#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif	

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include "ImGuiFileDialog.h"

#include "fonts/rubik.h"
#include "ThemeManager/themes.h"

#define XAMPP_ID 0
#define WAMP_ID 1

struct WebServerInfo {
    bool xamppFound = false;
    bool wampFound = false;

    std::string xamppPath;
    std::string wampPath;
    std::string xamppConfigPath;
    std::string wampConfigPath;
    std::string xamppServiceName = "Apache2.4";
    std::string wampServiceName = "wampapache64";
};

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

WebServerInfo detectWebServers() {
    WebServerInfo info;

    if (fs::exists("C:\\xampp\\apache\\conf\\extra\\httpd-vhosts.conf")) {
        info.xamppFound = true;
        info.xamppPath = "C:\\xampp";
        info.xamppConfigPath = "C:\\xampp\\apache\\conf\\extra\\httpd-vhosts.conf";
    }
    
    if (fs::exists("C:\\wamp64\\bin\\apache")) {
        for (const auto& entry : fs::directory_iterator("C:\\wamp64\\bin\\apache")) {
            if (entry.is_directory()) {
                std::string vhostPath = entry.path().string() + "\\conf\\extra\\httpd-vhosts.conf";
                if (fs::exists(vhostPath)) {
                    info.wampFound = true;
                    info.wampPath = "C:\\wamp64";
                    info.wampConfigPath = vhostPath;
                    break;
                }
            }
        }
    }
    
    return info;
}

bool writeToConfig(int configID, const WebServerInfo& info, const std::string &projectName) {
    std::string filename;
    std::string textToAppend;

    if (configID != XAMPP_ID && configID != WAMP_ID) {
        return false;
    }

    if (configID == XAMPP_ID) {
        if (!info.xamppFound) {
            return false;
        }
        filename = info.xamppConfigPath;
        textToAppend = "\nxampp!";
    }

    if (configID == WAMP_ID) {
        if (!info.wampFound) {
            return false;
        }
        filename = info.wampConfigPath;
        textToAppend = "wamp!";
    }

    std::ofstream outfile(filename, std::ios::app);
    if (!outfile.is_open()) {
        std::cerr << "Unable to open file: " << filename << "\n";
        return false;
    }


    outfile << textToAppend << "\n";
    outfile.close();
    return true;
}

int main() {
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    WebServerInfo serverInfo = detectWebServers();

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
std::string placeholderText = "";
int selectedServer = -1; //none

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

        float panelWidth = ImGui::GetContentRegionAvail().x * 1.0f;
        float panelHeight = ImGui::GetContentRegionAvail().y * 1.0f;

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
            if (selectedServer == -1) {
                placeholderText = "Please select a web server first.";
            } else {
                bool success = writeToConfig(selectedServer, serverInfo, projectName);
                if (success) {
                    placeholderText = "Virtual host written to config.";
                } else {
                    placeholderText = "Failed to write to config file.";
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Restart Web Server", ImVec2(150, 0))) {
            
        }
        
        ImGui::Dummy(ImVec2(0, 15)); 
        float statusTextWidth = ImGui::CalcTextSize("Web Server Status: Not Found").x;

        bool anyServerFound = serverInfo.xamppFound || serverInfo.wampFound;
        std::string statusText = "Web Server Status: ";

        if (serverInfo.xamppFound && serverInfo.wampFound) {
            statusText += "XAMPP & WAMP Found";
        } else if (serverInfo.xamppFound) {
            statusText += "XAMPP Found";
        } else if (serverInfo.wampFound) {
            statusText += "WAMP Found";
        } else {
            statusText += "Not Found";
        }
        ImGui::Dummy(ImVec2(0, 10));

        if (serverInfo.xamppFound) {
            ImGui::RadioButton("XAMPP", &selectedServer, XAMPP_ID);
        }
        if (serverInfo.wampFound) {
            ImGui::RadioButton("WAMP", &selectedServer, WAMP_ID);
        }
        ImGui::Dummy(ImVec2(0, 20));

        ImGui::TextColored(anyServerFound ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 
                        "%s", statusText.c_str());

        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.7f), placeholderText.c_str());

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