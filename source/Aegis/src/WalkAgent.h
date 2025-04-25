#pragma once

#include "NavAgent.h"

class WalkAgent : public NavAgent
{
public:
    WalkAgent(World* world);
protected:
    World* world;
protected:
    virtual navnode_t* NavNodeFromPos(Vector3 pos) override;
public:
    virtual bool ConstructPath(Vector3 start, Vector3 end) override;
};