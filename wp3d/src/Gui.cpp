#include "Gui.h"

#include <chrono>
#include <string>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>
#include <imgui_stdlib.h>
#include <ImGuiFileDialog.h>

#include "GuiElementEntityEditor.h"
#include "GuiElementMenuBar.h"
#include "GuiElementTextureBrowser.h"
#include "GuiElementTextureMapping.h"
#include "GuiElementToolBar.h"
#include "GuiElementToolSettings.h"
#include "GuiElementViewport.h"
#include "Map.h"

void Gui::Setup(GLFWwindow* win)
{
    int i;

    ImFont* opensans;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.Fonts->AddFontFromFileTTF("resource/OpenSans-Regular.ttf");

    ImGui::StyleColorsDark();
    ApplyStyle();
    
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL2_Init();

    map.NewMap();

    this->elements.push_back(std::make_unique<GuiElementMenuBar>(GuiElementMenuBar(this->map)));
    this->elements.push_back(std::make_unique<GuiElementToolBar>(GuiElementToolBar(this->map)));
    this->elements.push_back(std::make_unique<GuiElementToolSettings>(GuiElementToolSettings(this->map)));
    this->elements.push_back(std::make_unique<GuiElementEntityEditor>(GuiElementEntityEditor(this->map)));
    this->elements.push_back(std::make_unique<GuiElementTextureBrowser>(GuiElementTextureBrowser(this->map)));
    this->elements.push_back(std::make_unique<GuiElementTextureMapping>(GuiElementTextureMapping(this->map)));
    for(i=0; i<Viewport::NTYPES; i++)
        this->elements.push_back(std::make_unique<GuiElementViewport>(GuiElementViewport(this->map, (Viewport::viewporttype_e) i)));
}

void Gui::ApplyStyle(void)
{
    const ImVec4 windowbg = ImVec4(0.15, 0.15, 0.15, 1.0);
    const ImVec4 tabbar = ImVec4(0.05, 0.05, 0.05, 1.0);
    const ImVec4 button = ImVec4(0.2, 0.2, 0.2, 1.0);
    const ImVec4 active = ImVec4(0.25, 0.25, 0.25, 1.0);
    const ImVec4 hover = ImVec4(0.3, 0.3, 0.3, 1.0);
    const ImVec4 border = ImVec4(0.5, 0.5, 0.5, 1.0);

    ImGuiStyle *style;

    style = &ImGui::GetStyle();
    style->Colors[ImGuiCol_WindowBg] = windowbg;
    style->Colors[ImGuiCol_TitleBg] = tabbar;
    style->Colors[ImGuiCol_TitleBgActive] = tabbar;
    style->Colors[ImGuiCol_Tab] = tabbar;
    style->Colors[ImGuiCol_TabHovered] = hover;
    style->Colors[ImGuiCol_TabActive] = windowbg;
    style->Colors[ImGuiCol_TabUnfocused] = tabbar;
    style->Colors[ImGuiCol_TabUnfocusedActive] = tabbar;
    style->Colors[ImGuiCol_Border] = border;
    style->Colors[ImGuiCol_TabSelectedOverline] = border;
    style->Colors[ImGuiCol_Button] = button;
    style->Colors[ImGuiCol_ButtonActive] = active;
    style->Colors[ImGuiCol_ButtonHovered] = hover;
    style->Colors[ImGuiCol_FrameBg] = button;
    style->Colors[ImGuiCol_FrameBgActive] = active;
    style->Colors[ImGuiCol_FrameBgHovered] = hover;
    style->Colors[ImGuiCol_Separator] = hover;
    style->Colors[ImGuiCol_SeparatorHovered] = hover;
    style->Colors[ImGuiCol_SeparatorActive] = hover;
    style->Colors[ImGuiCol_ResizeGrip] = active;
    style->FrameRounding = 2;
    style->FrameBorderSize = 1;
    style->WindowMenuButtonPosition = ImGuiDir_None;
    style->TabBarOverlineSize = 0;
    style->TabBorderSize = 1;
}

void Gui::Draw()
{
    int i;

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);

    for(i=0; i<this->elements.size(); i++)
        this->elements[i]->Draw();

    // ImGui::ShowDemoWindow();
}

void Gui::FinishFrame()
{
    GLFWwindow *ctx;

    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    
    ctx = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(ctx);
}
