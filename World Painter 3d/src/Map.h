#pragma once

#include <imgui.h>

#include "Entity.h"
#include "Viewport.h"

class Map
{
public:
    typedef enum
    {
        TOOL_SELECT,
        TOOL_TRANSLATE,
        TOOL_ROTATE,
        TOOL_SCALE,
        TOOL_BRUSH,
    } tooltype_e;
private:
    void SetupFrame(const Viewport& view);

    // outbasis is forward, right, up
    void GetViewBasis(const Viewport& view, Eigen::Vector3f outbasis[3]);

    void PanOrtho(Viewport& view, ImGuiKey key);
    void MoveFreecam(Viewport& view, ImGuiKey key, float deltatime);
    void LookFreecam(Viewport& view, ImGuiKey key, float deltatime);
    void FinalizeBrush(void);

    void DrawGrid(const Viewport& view);
    void DrawWorkingBrush(const Viewport& view);
    void DrawDashedLine(Eigen::Vector3i l[2], float dashlen);
public:
    static constexpr float max_map_size = 8192.0f; // Maximum size of the map in any direction
    static constexpr int max_grid_level = 10; // 2^10 = 1024

    unsigned int gridlevel = 4; // 0 to max_grid_level

    int nbrushcorners = 0;
    Eigen::Vector3i brushcorners[2]; // once both corners are placed, it will be [min, max]
    tooltype_e tool;

    std::vector<Entity> entities;

    void SwitchTool(tooltype_e type);
    void KeyDown(Viewport& view, ImGuiKey key, float deltatime);
    void KeyPress(Viewport& view, ImGuiKey key);
    void Click(const Viewport& view, const Eigen::Vector2f& mousepos, ImGuiMouseButton_ button);
    void Render(const Viewport& view);

    void NewMap(void);
};