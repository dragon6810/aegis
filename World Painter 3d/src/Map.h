#pragma once

#include <imgui.h>

#include "Entity.h"
#include "Viewport.h"

class Map
{
private:
    void SetupFrame(const Viewport& view);

    // outbasis is forward, right, up
    void GetViewBasis(const Viewport& view, Eigen::Vector3f outbasis[3]);

    void PanOrtho(Viewport& view, ImGuiKey key);
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

    std::vector<Entity> entities;

    void KeyPress(Viewport& view, ImGuiKey key);
    void Click(const Viewport& view, const Eigen::Vector2f& mousepos, ImGuiMouseButton_ button);
    void Render(const Viewport& view);

    void NewMap(void);
};