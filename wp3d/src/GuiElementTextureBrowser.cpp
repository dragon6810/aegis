#include "GuiElementTextureBrowser.h"

GuiElementTextureBrowser::GuiElementTextureBrowser(Map& map) : GuiElement(map), map(map)
{
    this->ReloadTpk();
}

void GuiElementTextureBrowser::GenTextures(void)
{
    int i;
    std::unordered_map<std::string, Tpklib::TpkTex>::iterator it;

    GLuint tex;

    for(it=this->tpk.tex.begin(); it!=this->tpk.tex.end(); it++)
    {
        glGenTextures(1, &tex);

        glBindTexture(GL_TEXTURE_2D, tex);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        if(0)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
            
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, it->second.size[0], it->second.size[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, it->second.data.data());

        this->gltex[it->first] = tex;
    }
}

void GuiElementTextureBrowser::ReloadTpk(void)
{
    std::unordered_map<std::string, GLuint>::iterator it;

    for(it=this->gltex.begin(); it!=this->gltex.end(); it++)
        glDeleteTextures(1, &it->second);
    this->gltex.clear();

    this->tpk.tex.clear();
    this->tpk.Open("aegis.tpk");
    this->tpk.LoadTex("");
    this->tpk.Close();

    this->GenTextures();
}

void GuiElementTextureBrowser::DrawTex(Tpklib::TpkTex* tex, GLuint glid)
{
    ImVec2 size;

    size.x = tex->size[0];
    size.y = tex->size[1];
    ImGui::Image((ImTextureID)(intptr_t)glid, size, {0, 1}, {1, 0});
}

void GuiElementTextureBrowser::Draw(void)
{
    std::unordered_map<std::string, GLuint>::iterator it;

    ImGui::Begin("Texture Browser", NULL, ImGuiWindowFlags_NoCollapse);

    for(it=this->gltex.begin(); it!=this->gltex.end(); it++)
        this->DrawTex(&this->tpk.tex[it->first], it->second);

    ImGui::End();
}