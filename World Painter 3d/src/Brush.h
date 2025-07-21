#pragma once

#include <unordered_set>
#include <vector>

#include "Plane.h"
#include "Viewport.h"

class Entity;

class Brush
{
private:
    int FindVertex(Eigen::Vector3f p);
    void DrawVertexPreview(const Map& map);
public:
    std::vector<Plane> planes;
    std::unordered_set<int> plselection;
    std::vector<Eigen::Vector3f> points; // this is what is modified in vertex edit mode before confirmation
    std::unordered_set<int> pointselection; // this is all messy, will change soon. must be in sync with planes indexselection

    bool drawvertexpreview;

    void MakeFaces(void);
    bool RayIntersect(Eigen::Vector3f o, Eigen::Vector3f d, float* dist);
    void Select(Eigen::Vector3f o, Eigen::Vector3f r, int index, int ent, Map& map);
    void SelectTriplane(Eigen::Vector3f o, Eigen::Vector3f r, const Map& map);
    void SelectVerts(Eigen::Vector3f o, Eigen::Vector3f r, const Map& map);
    void Draw(const Viewport& view, int index, int ent, Map& map);
};
