#pragma once

#include <vector>
#include <unordered_map>
#include <utility>

#include "Vector.h"

// forward declarations
class World;
struct plane_t;
struct texinfo_t;
struct surf_t;
struct leaf_t;
struct node_t;
struct hullnode_t;
struct traceresult_t;

typedef struct navnode_s
{
    std::vector<Vector3> points;
    Vector3 normal;
    Vector3 center;
    std::vector<struct navnode_s*> edges;
} navnode_t;

class Nav
{
public:
    void Initialize(World* world);
    void Render(void);
private:
    World* world;

    std::vector<navnode_t> surfs[4];

    const float maxslope = 0.45;

    void FindSurfs(int hull);
    void FindEdges(int hull);

    std::vector<navnode_t> Expand(int hull);
    std::vector<navnode_t> CutPlanes(const std::vector<navnode_t>& surfs);
    std::vector<navnode_t> PruneFaces(const std::vector<navnode_t>& surfs);
    bool SurfQualifies(const navnode_t& surf);
    void DrawSurf(navnode_t* surf);
    bool SameEdge(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4);

    std::vector<std::array<Vector3, 3>> EarClip(std::vector<Vector3> poly);
    std::pair<Vector3, Vector3> MakeEdge(Vector3 a, Vector3 b);
};
