#pragma once

#include "NavAgent.h"

class WalkAgent : public NavAgent
{
public:
    const navnode_t* NavNodeFromPos(Vector3 pos);
};