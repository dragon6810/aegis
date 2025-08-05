#include "GuiElementTextureBrowser.h"

#include <filesystem>

GuiElementTextureBrowser::GuiElementTextureBrowser(Map& map) : GuiElement(map), map(map)
{
    
}

void GuiElementTextureBrowser::DrawTex(TextureManager::texture_t* tex, float width, int id, int column, int row, int ncolumns)
{
    ImVec2 size, pos, min, max, bgmin, bgmax, textsize, textpos;
    float scalefactor;
    ImDrawList *drawlist;
    bool selected;
    std::string displayname;

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
    ImGui::Image((ImTextureID)(intptr_t)tex->glid, {size.x - pad * 2, size.y - pad * 2 }, {0, 1}, {1, 0});

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
    int i, j, id;
    TextureManager::archive_t *archive;
    std::map<std::string, TextureManager::texture_t>::iterator it;

    float width;

    ImGui::Begin("Texture Browser", NULL, ImGuiWindowFlags_NoCollapse);
    
    ImGui::BeginChild("TextureList", 
        { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing() });

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    width = (ImGui::GetContentRegionAvail().x) / 2.0;

    for(i=j=id=0, archive=this->map.texmanager.archives.data(); i<this->map.texmanager.archives.size(); i++, archive++)
    {
        for(it=archive->textures.begin(); it!=archive->textures.end(); it++, id++)
        {
            if(!filter.PassFilter(it->first.c_str()))
                continue;

            this->DrawTex(&it->second, width, id, j & 1, j / 2, 2);
            j++;
        }
    }

    ImGui::Dummy({0, 0});

    ImGui::PopStyleVar();

    ImGui::EndChild();

    ImGui::PushItemWidth(-FLT_MIN);
    filter.Draw("##texbrowserfilter");
    ImGui::PopItemWidth();

    ImGui::End();
}