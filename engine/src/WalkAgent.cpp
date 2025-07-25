#include "WalkAgent.h"

#include "PolyMath.h"

#include "Game.h"

#define LOGERRORS

WalkAgent::WalkAgent(World* world)
{
    this->world = world;
}

navnode_t* WalkAgent::NavNodeFromPos(Eigen::Vector3f pos)
{
    const float seglength = 512.0;

    int i;

    NavMesh *navmesh;
    navnode_t *curnode;
    Eigen::Vector3f start, end;

    start = pos;
    end = start + Eigen::Vector3f(0, 0, -seglength);

    // TODO: Make faster, ideally use bsp or make an octree

    for(i=0; i<world->navmesh.surfs[0].size(); i++)
    {
        curnode = &world->navmesh.surfs[0][i];

        if(!PolyMath::SegmentIntersects(curnode->points, start, end))
            continue;

        return curnode;
    }

    return NULL;
}

bool WalkAgent::ConstructPath(Eigen::Vector3f start, Eigen::Vector3f end)
{
    int i;

    navnode_t *startnode, *endnode;
    std::vector<navnode_t*> nodepath;

    startnode = NavNodeFromPos(start);
    endnode = NavNodeFromPos(end);

    if(!startnode || !endnode)
    {
#ifdef LOGERRORS
        Console::Print("WalkAgent::ConstructPath: Failed to find valid start/end node!\n");
#endif
        return false;
    }

    nodepath = AStar(&this->world->navmesh, startnode, endnode);
    if(nodepath.size() < 2)
        return false;

    this->curpath.anchors.resize(nodepath.size());
    for(i=0; i<nodepath.size(); i++)
    {
        this->curpath.anchors[i].pos = nodepath[i]->center;
        this->curpath.anchors[i].node = nodepath[i];
    }

    this->curpath.anchors.front().pos = start;
    this->curpath.anchors.back().pos = end;

    /*
    this->curpath.anchors.resize(2);
    this->curpath.anchors[0].pos = startnode->center;
    this->curpath.anchors[0].node = startnode;
    this->curpath.anchors[1].pos = endnode->center;
    this->curpath.anchors[1].node = endnode;
    */
}