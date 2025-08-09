#pragma once

#include "BspPlane.h"
#include "Entity.h"
#include "HullDef.h"
#include "TexInfo.h"

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
    std::vector<BspPlane> planes;
    std::vector<TexInfo> texinfos;

    // if negative, normal is opposite. bitwise inverse for real index
    int FindPlaneNum(Eigen::Vector3f n, float d);
    int FindTexinfoNum(const char* name, Eigen::Vector3f s, Eigen::Vector3f t, float sshift, float tshift);

    void LoadMapFile(const char* path);
    void CSG(void);
};