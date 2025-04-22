#pragma once

#include "NavAgent.h"

class WalkAgent : public NavAgent
{
public:
    WalkAgent(World* world);
protected:
    World* world;
protected:
    navnode_t* NavNodeFromPos(Vector3 pos);
public:
    virtual bool ConstructPath(Vector3 start, Vector3 end) override;
};