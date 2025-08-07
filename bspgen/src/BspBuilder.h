#pragma once

#include "Entity.h"

class BspBuilder
{
public:
    bool verbose = false;
    std::string csgoutput = "";

    std::vector<Entity> ents;

    void LoadMapFile(const char* path);
    void CSG(void);
};