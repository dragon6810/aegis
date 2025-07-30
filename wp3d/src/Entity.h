#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Brush.h"
#include "Viewport.h"

class Entity
{
public:
    std::unordered_map<std::string, std::string> pairs;
    std::vector<Brush> brushes;
    std::unordered_set<int> brselection;
    
    void Select(Eigen::Vector3f o, Eigen::Vector3f r, int index, Map& map);
    void SelectVertex(Eigen::Vector3f o, Eigen::Vector3f r, const Map& map, const Viewport& view);
    bool RayIntersects(Eigen::Vector3f o, Eigen::Vector3f r, float* dist);
    void Draw(const Viewport& view, int index, Map& map);
};