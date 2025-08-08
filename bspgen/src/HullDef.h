#pragma once

#include <Eigen/Dense>

#include <bsplib.h>

class HullDef
{
public:
    HullDef(void);

    Eigen::Vector3f hulls[Bsplib::n_hulls][2];

    void LoadDefaults(void);
    bool LoadFile(const char* path);
};