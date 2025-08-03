#include "GuiElementToolSettings.h"

GuiElementToolSettings::GuiElementToolSettings(Map& map) : GuiElement(map), map(map)
{

}

void GuiElementToolSettings::SettingsSelect(void)
{
    const char *selecttypenames[] = { "Select Faces", "Select Brushes", "Select Entities", };
    int i;

    int selectiontype;

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
}

void GuiElementToolSettings::SettingsEntity(void)
{
    int i;
    std::set<int>::iterator it;

    Fgdlib::FgdFile *fgd;
    std::set<int> *pointents;

    ImGui::Text("Entity Class");

    fgd = &map.fgd;
    pointents = &fgd->entclasses[Fgdlib::EntityDef::ENTTYPE_POINT];
    it = pointents->begin();
    std::advance(it, map.workingenttype);
    if (ImGui::BeginCombo("##SelectionModeDropdown", fgd->ents[*it].classname.c_str()))
    {
        for (it=pointents->begin(), i=0; it!=pointents->end(); it++, i++)
        {
            if (ImGui::Selectable(fgd->ents[*it].classname.c_str(), map.workingenttype == i))
                map.workingenttype = i;
            if (map.workingenttype == i)
                ImGui::SetItemDefaultFocus();

            if (ImGui::IsItemHovered() && fgd->ents[*it].description.size())
                ImGui::SetTooltip(fgd->ents[*it].description.c_str());
        }
        ImGui::EndCombo();
    }
}

void GuiElementToolSettings::Draw(void)
{
    ImGui::Begin("Tool Settings", NULL, ImGuiWindowFlags_NoCollapse);

    switch(map.tool)
    {
    case Map::TOOL_SELECT:
        this->SettingsSelect();
        break;
    case Map::TOOL_ENTITY:
        this->SettingsEntity();
        break;
    default:
        break;
    }

    ImGui::End();
}