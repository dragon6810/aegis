#pragma once

#include "vector"

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
    std::vector<struct navnode_s*> edges;
} navnode_t;

class Nav
{
public:
    void Initialize(World* world);
    void Render(void);
private:
    World* world;

    std::vector<navnode_t> surfs;

    const float maxslope = 0.45;

    void FindSurfs(void);

    void NavSurfsFromSurf(struct surf_t* surf);
    bool SurfQualifies(struct surf_t* surf);
    void DrawSurf(navnode_t* surf);

    std::vector<std::array<Vector3, 3>> EarClip(std::vector<Vector3> poly);
};