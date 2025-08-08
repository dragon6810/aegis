#pragma once

#include "Entity.h"
#include "HullDef.h"

class BspBuilder
{
private:
    void ExpandHulls(void);
    void CullInterior(void);
    void WriteCSGFaces(void);
public:
    bool verbose = false;
    std::string csgoutput = "";

    HullDef hulldef;

    std::vector<Entity> ents;

    void LoadMapFile(const char* path);
    void CSG(void);
};