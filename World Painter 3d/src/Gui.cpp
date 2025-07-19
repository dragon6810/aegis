#include "Gui.h"

#include <string>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>

#include "Map.h"

void Gui::Setup(GLFWwindow* win)
{
    int i;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    
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

    currenttool = TOOL_BRUSH;
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
        ImGui::Begin(viewportname.c_str(), NULL, ImGuiWindowFlags_NoTitleBar);

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

            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) 
            {

            }
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {

            }
            if (ImGui::MenuItem("Save as", "Ctrl+Alt+S")) 
            {

            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    dockspaceflags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);

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