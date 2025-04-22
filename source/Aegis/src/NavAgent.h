#pragma once

#include <vector>

#include "Vector.h"

#include "NavMesh.h"

class NavAgent
{
public:
    typedef struct navanchor_s
    {
        Vector3 pos;
    } navanchor_t;

    typedef struct navpath_s
    {
        std::vector<navanchor_t> anchors;
    } navpath_t;
public:
    const virtual navnode_t* NavNodeFromPos(Vector3 pos) = 0;

    void RenderPath(const navpath_t& path);
};