#pragma once

#include <vector>

#include <Eigen/Dense>

#include "NavMesh.h"

class NavAgent
{
public:
    typedef struct navanchor_s
    {
        Eigen::Vector3f pos;
        navnode_t *node;
    } navanchor_t;

    typedef struct navpath_s
    {
        std::vector<navanchor_t> anchors;
    } navpath_t;
public:
    navpath_t curpath;
protected:
    virtual navnode_t* NavNodeFromPos(Eigen::Vector3f pos) = 0;
    virtual std::vector<navnode_t*> AStar(NavMesh* mesh, navnode_t* start, navnode_t* end);
public:
    virtual bool ConstructPath(Eigen::Vector3f start, Eigen::Vector3f end) = 0;

    void RenderPath();
};