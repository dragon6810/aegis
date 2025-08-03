#pragma once

#include <string>
#include <map>
#include <unordered_set>

#include <fgdlib.h>

#include "Brush.h"
#include "Viewport.h"

class Entity
{
private:
    void DrawCuboid(bool selected);
public:
    static constexpr float default_draw_radius = 8.0;

    std::map<std::string, std::string> pairs;
    std::vector<Brush> brushes;
    std::unordered_set<int> brselection;
    
    void Select(Eigen::Vector3f o, Eigen::Vector3f r, int index, Map& map);
    void SelectVertex(Eigen::Vector3f o, Eigen::Vector3f r, const Map& map, const Viewport& view);
    bool RayIntersects(Eigen::Vector3f o, Eigen::Vector3f r, float* dist);
    void FillDefaultPairs(Fgdlib::EntityDef* def, bool overwrite=false);
    Eigen::Vector3f GetOrigin(void);
    Fgdlib::EntityDef* GetDef(Fgdlib::FgdFile* file);
    void DeleteSelected(Map& map);
    void Draw(const Viewport& view, int index, Map& map);
};