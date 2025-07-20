#pragma once

#include <unordered_set>
#include <vector>

#include "Plane.h"
#include "Viewport.h"

class Entity;

class Brush
{
public:
    std::vector<Plane> planes;
    std::unordered_set<int> plselection;

    void MakeFaces(void);
    bool RayIntersect(Eigen::Vector3f o, Eigen::Vector3f d, float* dist);
    void Select(Eigen::Vector3f o, Eigen::Vector3f r, int index, int ent, Map& map);
    void Draw(const Viewport& view, int index, int ent, const Map& map);
};
