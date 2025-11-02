#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
#endif

#include <cstdlib>
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
#include "ServerManager/server_manager.hpp"

logger logger;
ServerManager serverManager;

std::string projectDirectory, projectName;
std::string placeholderText;

std::string extractProjectName(const std::string& path) {
    return fs::path(path).filename().string();
}

int main() {
    if(!glfwInit()) {
        logger.error("Failed to initialize GLFW.");
        return -1;
    }

    WebServerInfo serverInfo = serverManager.detectWebServers();
    logger.consoleLoggingEnabled = false;
    logger.fileLoggingEnabled = true;

// window properties
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // no old OpenGL

// window creation
    GLFWwindow* window;
    window = glfwCreateWindow(750, 550, "HostCTL - A simple local network manager for XAMPP, WAMP & MAMP (Pro)", NULL, NULL);

    if(window == NULL) {
        logger.error("Failed to create GLFW window. OpenGL 3.3 support is required!");
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

    ImFontConfig cfg16;
    cfg16.FontDataOwnedByAtlas = false;
    ImFont* font16 = io.Fonts->AddFontFromMemoryTTF((void*)Rubik, Rubik_len, 16.0f, &cfg16);

    #ifdef __WIN32__
        ImFontConfig cfg18;
        cfg18.FontDataOwnedByAtlas = false;
        ImFont* subTitleFont = io.Fonts->AddFontFromMemoryTTF((void*)Rubik, Rubik_len, 18.0f, &cfg18);
    #endif

    ImFontConfig cfg24;
    cfg24.FontDataOwnedByAtlas = false;
    ImFont* titleFont = io.Fonts->AddFontFromMemoryTTF((void*)Rubik, Rubik_len, 24.0f, &cfg24);

    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

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
        //ImGui::SetNextWindowViewport(viewport->ID);

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
        #ifdef __WIN32__
        ImGui::PushFont(subTitleFont);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,255,255,130));
        ImGui::Text("Note: If you have xampp AND wamp, the host will be written to both because I am lazy..");
        ImGui::PopStyleColor();
        ImGui::PopFont();
        #endif
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 15));

        ImGui::Text("Local Domain Name:");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (150 - ImGui::CalcTextSize("Local Domain Name:").x));
        ImGui::SetNextItemWidth(220);
        ImGui::InputText("##domainInput", &projectName);
        ImGui::Dummy(ImVec2(0, 10));

        ImGui::Text("Folder Location:");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (150 - ImGui::CalcTextSize("Folder Location:").x));
        ImGui::SetNextItemWidth(320);
        ImGui::InputText("##folderInput", &projectDirectory);
        ImGui::SameLine();
        if (ImGui::Button("Browse")) {
            IGFD::FileDialogConfig config;
	        config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose Directory", nullptr, config);
        }
        ImGui::Dummy(ImVec2(0, 20));

        if (ImGui::Button("Create Host", ImVec2(120, 0))) {
            serverManager.projectName = projectName;
            serverManager.projectDirectory = projectDirectory;
            placeholderText += serverManager.createHost(serverInfo);
        }
        ImGui::SameLine();
        if (ImGui::Button("Restart Web Server", ImVec2(150, 0))) {
            serverManager.restartApache(serverInfo, placeholderText);
        }

        ImGui::Dummy(ImVec2(0, 10));

        bool anyServerFound = serverInfo.xamppFound || serverInfo.wampFound || serverInfo.mampFound;
        std::string statusText = "Web Server Status: ";

        if (serverInfo.xamppFound && serverInfo.wampFound) {
            statusText += "XAMPP & WAMP Found";
        } else if (serverInfo.xamppFound) {
            statusText += "XAMPP Found";
        } else if (serverInfo.wampFound) {
            statusText += "WAMP Found";
        } else if (serverInfo.mampFound) {
            statusText += "MAMP (Pro) found";
        } else {
            statusText += "Not Found";
        }
        ImGui::TextColored(anyServerFound ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
        "%s", statusText.c_str());

        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.7f), "%s", placeholderText.c_str());

        ImGui::EndChild();

        //file dialog destreuction
        if (ImGuiFileDialog::Instance()->Display("ChooseDirDlgKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                projectDirectory = filePath;
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
