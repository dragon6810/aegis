#include "GuiElementTextureBrowser.h"

#include <filesystem>

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
    std::map<std::string, GLuint>::iterator it;

    for(it=this->gltex.begin(); it!=this->gltex.end(); it++)
        glDeleteTextures(1, &it->second);
    this->gltex.clear();

    this->tpk.tex.clear();
    this->tpk.Open("aegis.tpk");
    this->tpk.LoadTex("");
    this->tpk.Close();

    this->GenTextures();
}

void GuiElementTextureBrowser::DrawTex(Tpklib::TpkTex* tex, GLuint glid, float width, int id, int column, int row, int ncolumns)
{
    ImVec2 size, pos, min, max, bgmin, bgmax, textsize, textpos;
    float scalefactor;
    ImDrawList *drawlist;
    bool selected;
    std::string dirname, displayname;

    dirname = std::filesystem::path(tex->filename).stem().string();
    if(dirname.size())
        displayname = dirname + "/" + tex->name;
    else
        displayname = tex->name;

    pos = { column * width, row * width };
    ImGui::SetCursorPos(pos);

    scalefactor = width / (float) tex->size[0];

    size.x = (float) tex->size[0] * scalefactor;
    size.y = (float) tex->size[1] * scalefactor;
    selected = id == this->selected;
    if(ImGui::Selectable((std::string("##texbrowser_") + displayname).c_str(), &selected, 0, size))
        this->selected = id;

    pos.x += pad;
    pos.y += pad;

    ImGui::SetCursorPos(pos);
    ImGui::Image((ImTextureID)(intptr_t)glid, {size.x - pad * 2, size.y - pad * 2 }, {0, 1}, {1, 0});

    drawlist = ImGui::GetWindowDrawList();
    min = ImGui::GetItemRectMin();
    max = ImGui::GetItemRectMax();

    ImGui::PushFont(NULL, 12);

    textsize = ImGui::CalcTextSize(displayname.c_str());

    textpos.x = max.x - textsize.x - pad;
    textpos.y = max.y - textsize.y - pad;

    bgmin.x = textpos.x - pad;
    bgmin.y = textpos.y - pad;
    bgmax.x = textpos.x + textsize.x + pad;
    bgmax.y = textpos.y + textsize.y + pad;

    drawlist->AddRectFilled(bgmin, bgmax, IM_COL32(0, 0, 0, 192));
    drawlist->AddText(textpos, IM_COL32(255, 255, 255, 255), displayname.c_str());

    ImGui::PopFont();
}

void GuiElementTextureBrowser::Draw(void)
{
    int i;
    std::map<std::string, GLuint>::iterator it;
    float width;

    ImGui::Begin("Texture Browser", NULL, ImGuiWindowFlags_NoCollapse);
    
    ImGui::BeginChild("TextureList", 
        { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing() });

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    width = (ImGui::GetContentRegionAvail().x) / 2.0;

    for(it=this->gltex.begin(), i=0; it!=this->gltex.end(); it++)
    {
        if(!filter.PassFilter(it->first.c_str()))
            continue;
        
        this->DrawTex(&this->tpk.tex[it->first], it->second, width, i, i & 1, i / 2, 2);
        i++;
    }

    ImGui::Dummy({0, 0});

    ImGui::PopStyleVar();

    ImGui::EndChild();

    ImGui::PushItemWidth(-FLT_MIN);
    filter.Draw("##texbrowserfilter");
    ImGui::PopItemWidth();

    ImGui::End();
}