#pragma once

#include "BrFace.h"

class Brush
{
public:
    std::vector<BrFace> planes;
    Eigen::Vector3f bb[2];

    void Polygonize(void);
    void FindBB(void);
    void Expand(const Eigen::Vector3f hull[2]);
};