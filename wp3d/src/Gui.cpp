#include "Gui.h"

#include <chrono>
#include <string>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>

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

    for(i=0; i<Viewport::NTYPES; i++)
    {
        this->viewports[i].fbo = 0;
        this->viewports[i].tex = 0;
        this->viewports[i].depth = 0;
        this->viewports[i].canvassize = Eigen::Vector2i(0, 0);
        this->viewports[i].type = (Viewport::viewporttype_e) i;
        if(i != Viewport::FREECAM)
            this->viewports[i].pos[i] = Map::max_map_size + 8.0;
        else
            this->viewports[i].wireframe = false;
    }
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

void Gui::ViewportInput(void)
{

}

void Gui::DrawViewports(float deltatime)
{
    int i, j;

    std::string viewportname;
    ImVec2 viewportsize, mousepos;

    this->currentviewport = -1;
    for(i=0; i<Viewport::NTYPES; i++)
    {
        viewportname = std::string("Viewport ") + std::to_string(i);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin(viewportname.c_str(), NULL, ImGuiWindowFlags_NoCollapse);

        if(ImGui::IsWindowFocused())
            this->currentviewport = i;

        viewportsize = ImGui::GetContentRegionAvail();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->viewports[i].fbo);
        if(this->viewports[i].canvassize.x() != viewportsize.x || this->viewports[i].canvassize.y() != viewportsize.y)
        {
            if (this->viewports[i].tex)
                glDeleteTextures(1, &this->viewports[i].tex);
            if (this->viewports[i].fbo)
                glDeleteFramebuffersEXT(1, &this->viewports[i].fbo);
            if (this->viewports[i].depth)
                glDeleteRenderbuffersEXT(1, &this->viewports[i].depth);

            glGenTextures(1, &this->viewports[i].tex);
            glGenFramebuffersEXT(1, &this->viewports[i].fbo);
            glGenRenderbuffersEXT(1, &this->viewports[i].depth);

            glBindTexture(GL_TEXTURE_2D, this->viewports[i].tex);
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->viewports[i].fbo);
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->viewports[i].depth);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewportsize.x, viewportsize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, viewportsize.x, viewportsize.y);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->viewports[i].tex, 0);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->viewports[i].depth);
            
            if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
                fprintf(stderr, "error creating fbo for viewport %d with code %d\n", i, glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT));

            this->viewports[i].canvassize = Eigen::Vector2i(viewportsize.x, viewportsize.y);
        }

        if(ImGui::IsWindowFocused())
        {
            for(j=ImGuiKey_NamedKey_BEGIN; j<ImGuiKey_NamedKey_END; j++)
            {
                if(ImGui::IsKeyPressed((ImGuiKey) j))
                    map.KeyPress(this->viewports[i], (ImGuiKey) j);
                if(ImGui::IsKeyDown((ImGuiKey) j))
                    map.KeyDown(this->viewports[i], (ImGuiKey) j, deltatime);
            }
            
            // mouse input only when the mouse is over the window
            if(ImGui::IsWindowHovered())
            {
                mousepos.x = ImGui::GetMousePos().x - ImGui::GetWindowPos().x;
                mousepos.y = ImGui::GetMousePos().y - ImGui::GetWindowPos().y;
                mousepos.y -= ImGui::GetWindowSize().y - viewportsize.y;
                mousepos.x /= viewportsize.x;
                mousepos.y /= viewportsize.y;
                mousepos.x =  (mousepos.x * 2.0 - 1.0);
                mousepos.y = -(mousepos.y * 2.0 - 1.0);

                for(j=ImGuiMouseButton_Left; j<ImGuiMouseButton_Middle; j++)
                {
                    if (!ImGui::IsMouseClicked(j))
                        continue;
                    
                    this->map.Click
                    (
                        this->viewports[i],
                        Eigen::Vector2f(mousepos.x, mousepos.y), 
                        (ImGuiMouseButton_) j
                    );
                }
            }
        }

        map.Render(this->viewports[i]);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        
        ImGui::Image((ImTextureID)(intptr_t)this->viewports[i].tex, viewportsize, {0, 1}, {1, 0});
        ImGui::End();
        ImGui::PopStyleVar();
    }
}

void Gui::DrawToolBar(void)
{
    const auto tool = [&](const char* label, Map::tooltype_e toolId, const char* shortcut, const char* description)
    {
        if (map.tool == toolId)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
        else
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);

        if (ImGui::Button(label, ImVec2(ImGui::GetContentRegionAvail().x, 0.0)))
            map.SwitchTool(toolId);

        ImGui::PopStyleColor();

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(shortcut);
            ImGui::TextUnformatted(description);
            ImGui::EndTooltip();
        }
    };

    bool shift;

    shift = ImGui::GetIO().KeyShift;
    if (shift && ImGui::IsKeyPressed(ImGuiKey_S)) map.SwitchTool(Map::TOOL_SELECT);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_W)) map.SwitchTool(Map::TOOL_TRANSLATE);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_E)) map.SwitchTool(Map::TOOL_ROTATE);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_R)) map.SwitchTool(Map::TOOL_SCALE);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_Z)) map.SwitchTool(Map::TOOL_VERTEX);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_Q)) map.SwitchTool(Map::TOOL_PLANE);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_B)) map.SwitchTool(Map::TOOL_BRUSH);

    ImGui::Begin("Tool Bar", NULL, ImGuiWindowFlags_NoCollapse);

    tool("Select",         Map::TOOL_SELECT,    "Select Tool (Shift + S)", "Can be used to select vertices,\nplanes, brushes, or entities");
    tool("Translate",      Map::TOOL_TRANSLATE, "Translate Tool (Shift + W)", "Can be used to translate vertices,\nplanes, brushes, or entities");
    tool("Rotate",         Map::TOOL_ROTATE,    "Rotate Tool (Shift + E)", "Can be used to rotate brushes or\nentities");
    tool("Scale",          Map::TOOL_SCALE,     "Scale Tool (Shift + R)", "Can be used to scale brushes");
    tool("Vertex Editing", Map::TOOL_VERTEX,    "Vertex Editing Tool (Shift + Z)", "Can be used to modify vertex geometry");
    tool("Plane",          Map::TOOL_PLANE,     "Brush Tool (Shift + Q)", "Can be used to modify or create new planes");
    tool("Brush",          Map::TOOL_BRUSH,     "Brush Tool (Shift + B)", "Can be used to create new brushes");

    ImGui::End();
}

void Gui::DrawToolSettings(void)
{
    const char *selecttypenames[] = { "Select Planes", "Select Brushes", "Select Entities", };

    int i;

    int selectiontype;

    ImGui::Begin("Tool Settings", NULL, ImGuiWindowFlags_NoCollapse);

    switch(map.tool)
    {
    case Map::TOOL_SELECT:
        ImGui::Text("Selection Mode");

        for(i=0; i<(int) Map::SELECT_COUNT; i++)
            if(ImGui::IsKeyPressed((ImGuiKey) ((int) ImGuiKey_1 + i)))
                    map.selectiontype = (Map::selectiontype_e) i;

        selectiontype = (int) map.selectiontype;
        if (ImGui::BeginCombo("##SelectionModeDropdown", selecttypenames[selectiontype]))
        {
            for (i=0; i<(int) Map::SELECT_COUNT; i++)
            {
                if (ImGui::Selectable(selecttypenames[i], selectiontype == i))
                    selectiontype = i;
                if (selectiontype == i)
                    ImGui::SetItemDefaultFocus();

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip((std::string(selecttypenames[i]) + " (" + std::to_string(i+1) + ")").c_str());
            }
            ImGui::EndCombo();
            map.selectiontype = (Map::selectiontype_e) selectiontype;
        }

        break;
    default:
        break;
    }

    ImGui::End();
}

void Gui::DrawRibbon(void)
{
    ImGui::Begin("Ribbon", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
    ImGui::End();
}

void Gui::Draw()
{
    float deltatime;
    uint64_t curframe, msdelta;
    ImGuiDockNodeFlags dockspaceflags;

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar()) 
    {
        if (ImGui::BeginMenu("File")) 
        {
            if (ImGui::MenuItem("New", "Ctrl+N")) 
            {
                map.NewMap();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);

    DrawRibbon();
    DrawToolBar();
    DrawToolSettings();

    curframe = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    if(!lastframe)
        lastframe = curframe;
    msdelta = curframe - lastframe;
    deltatime = (float) msdelta / 1000.0;

    this->DrawViewports(deltatime);

    lastframe = curframe;
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
