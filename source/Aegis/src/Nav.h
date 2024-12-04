#pragma once

#include "vector"

// forward declarations
class World;
struct plane_t;
struct texinfo_t;
struct surf_t;
struct leaf_t;
struct node_t;
struct hullnode_t;
struct traceresult_t;

class Nav
{
public:
    void Initialize(World* world);
    void Render(void);
private:
    World* world;

    std::vector<struct surf_t*> surfs;

    const float maxslope = 0.45;

    void FindSurfs(void);

    bool SurfQualifies(struct surf_t* surf);
    void DrawSurf(struct surf_t* surf);
};