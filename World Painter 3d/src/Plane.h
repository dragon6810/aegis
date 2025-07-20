#pragma once

#include <string.h>

#include <Eigen/Dense>

#include "Polygon.h"
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
    Polygon poly; // cached, must be updated when brush is updated

    bool RayIntersectFace(Eigen::Vector3f o, Eigen::Vector3f d, float* dist);
    void Select(Eigen::Vector3f o, Eigen::Vector3f r, int index, int brush, int ent, Map& map);
    void Draw(const Viewport& view, int index, int brush, int ent, const Map& map);
};