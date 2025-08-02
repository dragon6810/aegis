#include "GuiElementToolBar.h"

GuiElementToolBar::GuiElementToolBar(Map& map) : GuiElement(map), map(map)
{

}

void GuiElementToolBar::DrawToolButton(const char* label, Map::tooltype_e type, const char* shortcut, const char* desc)
{
    if (map.tool == type)
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
    else
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);

    if (ImGui::Button(label, ImVec2(ImGui::GetContentRegionAvail().x, 0.0)))
        map.SwitchTool(type);

    ImGui::PopStyleColor();

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s (%s)\n", label, shortcut);
        ImGui::Separator();
        ImGui::TextUnformatted(desc);
        ImGui::EndTooltip();
    }
}

void GuiElementToolBar::Draw(void)
{
    bool shift;

    shift = ImGui::IsKeyDown(ImGuiKey_ModShift);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_S)) map.SwitchTool(Map::TOOL_SELECT);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_W)) map.SwitchTool(Map::TOOL_TRANSLATE);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_E)) map.SwitchTool(Map::TOOL_ROTATE);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_R)) map.SwitchTool(Map::TOOL_SCALE);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_Z)) map.SwitchTool(Map::TOOL_VERTEX);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_X)) map.SwitchTool(Map::TOOL_PLANE);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_B)) map.SwitchTool(Map::TOOL_BRUSH);

    ImGui::Begin("Tool Bar", NULL, ImGuiWindowFlags_NoCollapse);

    DrawToolButton("Select",         Map::TOOL_SELECT,    "Shift S", "Can be used to select planes, brushes, or entities");
    DrawToolButton("Translate",      Map::TOOL_TRANSLATE, "Shift W", "Can be used to translate brushes or entities");
    DrawToolButton("Rotate",         Map::TOOL_ROTATE,    "Shift E", "Can be used to rotate brushes or entities");
    DrawToolButton("Scale",          Map::TOOL_SCALE,     "Shift R", "Can be used to scale brushes or brush entities");
    DrawToolButton("Vertex Editing", Map::TOOL_VERTEX,    "Shift V", "Can be used to modify vertex geometry");
    DrawToolButton("Plane",          Map::TOOL_PLANE,     "Shift X", "Can be used to create new planes");
    DrawToolButton("Brush",          Map::TOOL_BRUSH,     "Shift B", "Can be used to create new brushes");

    ImGui::End();
}