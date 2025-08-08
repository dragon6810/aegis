#include "GuiElementTextureBrowser.h"

#include <filesystem>

GuiElementTextureBrowser::GuiElementTextureBrowser(Map& map) : GuiElement(map), map(map)
{
    
}

void GuiElementTextureBrowser::DrawTex(TextureManager::texture_t* tex, float width, int iarchive, int id, int column, int row, int ncolumns)
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
    selected = map.selectedtexarchive == iarchive && map.selectedtex == id;
    if(ImGui::Selectable((std::string("##texbrowser_") + displayname).c_str(), &selected, 0, size))
    {
        map.selectedtexarchive = iarchive;
        map.selectedtex = id;
    }

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

void GuiElementTextureBrowser::ApplyTex(void)
{
    int i;
    std::unordered_set<int>::iterator it;

    TextureManager::archive_t *archive;
    TextureManager::texture_t *selectedtex;
    std::map<std::string, TextureManager::texture_t>::iterator texit;

    if(map.selectedtexarchive >= map.texmanager.archives.size())
        return;
    archive = &map.texmanager.archives[map.selectedtexarchive];
    if(map.selectedtex >= archive->textures.size())
        return;
    texit = archive->textures.begin();
    std::advance(texit, map.selectedtex);
    selectedtex = &texit->second;

    if(this->map.selectiontype == Map::SELECT_ENTITY)
    {
        for(it=this->map.entselection.begin(); it!=this->map.entselection.end(); it++)
            this->map.entities[*it].ApplyTexture(selectedtex->name.c_str());
        return;
    }

    for(i=0; i<this->map.entities.size(); i++)
        this->map.entities[i].ApplyTextureToSelected(map, selectedtex->name.c_str());
}

void GuiElementTextureBrowser::Draw(void)
{
    int i, j, id;
    TextureManager::archive_t *archive;
    std::map<std::string, TextureManager::texture_t>::iterator it;

    float width;

    ImGui::Begin("Texture Browser", NULL, ImGuiWindowFlags_NoCollapse);
    
    ImGui::BeginChild("TextureList", 
        { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing() * 2 });

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    width = (ImGui::GetContentRegionAvail().x) / 2.0;

    for(i=j=id=0, archive=this->map.texmanager.archives.data(); i<this->map.texmanager.archives.size(); i++, archive++)
    {
        for(it=archive->textures.begin(); it!=archive->textures.end(); it++, id++)
        {
            if(!filter.PassFilter(it->first.c_str()))
                continue;

            this->DrawTex(&it->second, width, i, id, j & 1, j / 2, 2);
            j++;
        }
    }

    ImGui::Dummy({0, 0});

    ImGui::PopStyleVar();

    ImGui::EndChild();

    ImGui::PushItemWidth(-FLT_MIN);
    filter.Draw("##texbrowserfilter");
    if(ImGui::Button("Apply Texture"))
        this->ApplyTex();
    ImGui::PopItemWidth();

    if(ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted("Apply Texture (Shift T)");
        ImGui::Separator();
        ImGui::TextUnformatted("Applies the selected texture to selection.");
        ImGui::EndTooltip();
    }

    ImGui::End();

    if(ImGui::IsKeyDown(ImGuiKey_ModShift) && ImGui::IsKeyDown(ImGuiKey_T))
        this->ApplyTex();
}