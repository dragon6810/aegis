#pragma once

#include <unordered_set>

#include <imgui.h>

#include <fgdlib.h>

#include "Entity.h"
#include "Viewport.h"

class Map
{
public:
    typedef enum
    {
        TOOL_SELECT=0,
        TOOL_TRANSLATE,
        TOOL_ROTATE,
        TOOL_SCALE,
        TOOL_VERTEX,
        TOOL_PLANE,
        TOOL_BRUSH,
        TOOL_COUNT,
    } tooltype_e;

    typedef enum
    {
        SELECT_PLANE=0,
        SELECT_BRUSH,
        SELECT_ENTITY,
        SELECT_COUNT,
    } selectiontype_e;
private:
    void SetupFrame(const Viewport& view);

    void PanOrtho(Viewport& view, ImGuiKey key);
    void MoveFreecam(Viewport& view, ImGuiKey key, float deltatime);
    void LookFreecam(Viewport& view, ImGuiKey key, float deltatime);
    void FinalizeBrush(void);
    void FinalizePlane(void);
    void ClearSelection(void);
    void MoveVertexPoints(Eigen::Vector3f add);
    void FinalizeVertexEdit(void);

    void DrawGrid(const Viewport& view);
    void DrawWorkingBrush(const Viewport& view);
    void DrawTriplane(const Viewport& view);
    void DrawDashedLine(Eigen::Vector3i l[2], float dashlen);
public:
    static constexpr float max_map_size = 8192.0f; // Maximum size of the map in any direction
    static constexpr int max_grid_level = 10; // 2^10 = 1024

    unsigned int gridlevel = 4; // 0 to max_grid_level

    int nbrushcorners = 0;
    Eigen::Vector3i brushcorners[2];
    int ntriplane = 0;
    bool drawingtriplane = false;
    Eigen::Vector3f triplane[3];
    std::unordered_set<int> triplaneselection;
    tooltype_e tool;
    selectiontype_e selectiontype;

    std::string path = "";

    Fgdlib::FgdFile fgd;

    std::vector<Entity> entities;
    std::unordered_set<int> entselection;

    void SwitchTool(tooltype_e type);
    void KeyDown(Viewport& view, ImGuiKey key, float deltatime);
    void KeyPress(Viewport& view, ImGuiKey key);
    void Click(const Viewport& view, const Eigen::Vector2f& mousepos, ImGuiMouseButton_ button);
    void MouseUpdate(const Viewport& view, const Eigen::Vector2f& mousepos);
    void Render(const Viewport& view);

    void NewMap(void);
    void Save(void);
    void Load(const std::string& path);
};