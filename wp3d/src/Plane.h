#pragma once

#include <string.h>
#include <unordered_set>

#include <Eigen/Dense>
#include <mathlib.h>

#include "Viewport.h"

class Brush;
class Map;

class Plane
{
private:
    void DrawWire(const Viewport& view, bool drawselected);
    void DrawShaded(const Viewport& view, bool drawselected);
public:
    Eigen::Vector3f normal = Eigen::Vector3f(0, 0, 0);
    float d = 0;
    std::string texture = "";
    Mathlib::Poly<3> poly; // cached, must be updated when brush is updated
    std::vector<int> indices; // indices into brush vertices for poly
    std::unordered_set<int> indexselection;

    bool RayIntersectFace(Eigen::Vector3f o, Eigen::Vector3f d, float* dist);
    void Select(Eigen::Vector3f o, Eigen::Vector3f r, int index, int brush, int ent, Map& map);
    void SelectVerts(Eigen::Vector3f o, Eigen::Vector3f r, Brush& brush, const Viewport& view);
    void Move(Eigen::Vector3f add);
    void Draw(const Viewport& view, int index, int brush, int ent, Map& map, bool drawselected);
};