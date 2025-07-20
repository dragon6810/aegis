#pragma once

#include <string.h>
#include <unordered_set>

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
    bool PointRay(Eigen::Vector3f o, Eigen::Vector3f r, Eigen::Vector3f p);
public:
    Eigen::Vector3f normal = Eigen::Vector3f(0, 0, 0);
    float d = 0;
    Eigen::Vector3f triplane[3] = {};
    std::unordered_set<int> triplaneselection;
    std::string texture = "";
    Polygon poly; // cached, must be updated when brush is updated

    void UpdateTriplane(void);
    bool RayIntersectFace(Eigen::Vector3f o, Eigen::Vector3f d, float* dist);
    void Select(Eigen::Vector3f o, Eigen::Vector3f r, int index, int brush, int ent, Map& map);
    void SelectTriplane(Eigen::Vector3f o, Eigen::Vector3f r);
    void Draw(const Viewport& view, int index, int brush, int ent, const Map& map);
};