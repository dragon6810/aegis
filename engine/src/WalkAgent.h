#pragma once

#include "NavAgent.h"

class WalkAgent : public NavAgent
{
public:
    WalkAgent(World* world);
protected:
    World* world;
protected:
    virtual navnode_t* NavNodeFromPos(Eigen::Vector3f pos) override;
public:
    virtual bool ConstructPath(Eigen::Vector3f start, Eigen::Vector3f end) override;
};