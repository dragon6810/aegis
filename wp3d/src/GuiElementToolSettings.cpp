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

void GuiElementToolSettings::Draw(void)
{
    ImGui::Begin("Tool Settings", NULL, ImGuiWindowFlags_NoCollapse);

    switch(map.tool)
    {
    case Map::TOOL_SELECT:
        this->SettingsSelect();
        break;
    default:
        break;
    }

    ImGui::End();
}