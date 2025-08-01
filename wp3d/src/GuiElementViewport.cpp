#include "GuiElementViewport.h"

GuiElementViewport::GuiElementViewport(Map& map, Viewport::viewporttype_e type) : GuiElement(map), map(map)
{
    this->view.fbo = 0;
    this->view.tex = 0;
    this->view.depth = 0;
    this->view.canvassize = Eigen::Vector2i(0, 0);
    this->view.type = type;
    this->view.zoom = 128.0;
    this->view.fov = 60.0;
    this->view.rot = Eigen::Vector3f::Zero();
    this->view.pos = Eigen::Vector3f::Zero();
    if(type != Viewport::FREECAM)
        this->view.pos[type] = Map::max_map_size + 8.0;
    else
        this->view.wireframe = false;

    this->lastframe = 0;
}

void GuiElementViewport::RegenFBO(void)
{
    if (this->view.tex)
        glDeleteTextures(1, &this->view.tex);
    if (this->view.fbo)
        glDeleteFramebuffersEXT(1, &this->view.fbo);
    if (this->view.depth)
        glDeleteRenderbuffersEXT(1, &this->view.depth);

    glGenTextures(1, &this->view.tex);
    glGenFramebuffersEXT(1, &this->view.fbo);
    glGenRenderbuffersEXT(1, &this->view.depth);

    glBindTexture(GL_TEXTURE_2D, this->view.tex);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->view.fbo);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->view.depth);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->view.canvassize[0], this->view.canvassize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, this->view.canvassize[0], this->view.canvassize[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->view.tex, 0);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->view.depth);

    if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
        fprintf(stderr, "error creating fbo for viewport %d with code %d\n", (int) this->view.type, glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT));
}

void GuiElementViewport::KeyboardInput(float deltatime)
{
    int i;

    for(i=ImGuiKey_NamedKey_BEGIN; i<ImGuiKey_NamedKey_END; i++)
    {
        if(ImGui::IsKeyPressed((ImGuiKey) i))
            map.KeyPress(this->view, (ImGuiKey) i);
        if(ImGui::IsKeyDown((ImGuiKey) i))
            map.KeyDown(this->view, (ImGuiKey) i, deltatime);
    }
}

void GuiElementViewport::MouseInput(void)
{
    int i;

    ImVec2 mousepos;

    mousepos.x = ImGui::GetMousePos().x - ImGui::GetWindowPos().x;
    mousepos.y = ImGui::GetMousePos().y - ImGui::GetWindowPos().y;
    mousepos.y -= ImGui::GetWindowSize().y - this->view.canvassize[1];
    mousepos.x /= this->view.canvassize[0];
    mousepos.y /= this->view.canvassize[1];
    mousepos.x =  (mousepos.x * 2.0 - 1.0);
    mousepos.y = -(mousepos.y * 2.0 - 1.0);

    this->map.MouseUpdate(this->view, Eigen::Vector2f(mousepos[0], mousepos[1]));

    for(i=ImGuiMouseButton_Left; i<ImGuiMouseButton_Middle; i++)
    {
        if (!ImGui::IsMouseClicked(i))
            continue;

        this->map.Click
        (
            this->view,
            Eigen::Vector2f(mousepos.x, mousepos.y), 
            (ImGuiMouseButton_) i
        );
    }
}

void GuiElementViewport::Draw(void)
{
    int i;

    std::string name;
    ImVec2 size;
    ImVec2 input;

    float deltatime;
    uint64_t curframe, msdelta;

    curframe = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    if(!lastframe)
        lastframe = curframe;
    msdelta = curframe - lastframe;
    deltatime = (float) msdelta / 1000.0;
    lastframe = curframe;

    name = std::string("Viewport ") + std::to_string((int) this->view.type);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_NoCollapse);

    size = ImGui::GetContentRegionAvail();
    if(this->view.canvassize.x() != size.x || this->view.canvassize.y() != size.y)
    {
        this->view.canvassize = Eigen::Vector2i(size[0], size[1]);
        this->RegenFBO();
    }

    if(ImGui::IsWindowFocused())
    {
        this->KeyboardInput(deltatime);
        
        if(ImGui::IsWindowHovered())
            this->MouseInput();
    }

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->view.fbo);

    map.Render(this->view);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    
    ImGui::Image((ImTextureID)(intptr_t)this->view.tex, size, {0, 1}, {1, 0});
    ImGui::End();
    ImGui::PopStyleVar();
}