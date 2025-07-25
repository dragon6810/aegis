#pragma once

#include <vector>
#include <unordered_map>
#include <utility>

#include <Eigen/Dense>

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
    std::vector<Eigen::Vector3f> points;
    Eigen::Vector3f normal;
    Eigen::Vector3f center;
    std::vector<struct navnode_s*> edges;

    // Pathfinding intermediate values
    struct navnode_s *exploredby;
    float h, f;
    bool explored;
} navnode_t;

class NavMesh
{
public:
    std::vector<navnode_t> surfs[4];

    void Initialize(World* world);
    void Render(void);
private:
    World* world;

    const float maxslope = 0.45;

    void FindSurfs(int hull);
    void FindEdges(int hull);

    std::vector<navnode_t> Expand(int hull);
    std::vector<navnode_t> CutPlanes(const std::vector<navnode_t>& surfs);
    std::vector<navnode_t> PruneFaces(const std::vector<navnode_t>& surfs);
    bool SurfQualifies(const navnode_t& surf);
    void DrawSurf(navnode_t* surf);
    bool SameEdge(Eigen::Vector3f v1, Eigen::Vector3f v2, Eigen::Vector3f v3, Eigen::Vector3f v4);

    std::vector<std::array<Eigen::Vector3f, 3>> EarClip(std::vector<Eigen::Vector3f> poly);
    std::pair<Eigen::Vector3f, Eigen::Vector3f> MakeEdge(Eigen::Vector3f a, Eigen::Vector3f b);
};
