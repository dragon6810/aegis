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
        navnode_t *node;
    } navanchor_t;

    typedef struct navpath_s
    {
        std::vector<navanchor_t> anchors;
    } navpath_t;
public:
    navpath_t curpath;
protected:
    const virtual navnode_t* NavNodeFromPos(Vector3 pos) = 0;
public:
    virtual bool ConstructPath(Vector3 start, Vector3 end) = 0;

    void RenderPath();
};