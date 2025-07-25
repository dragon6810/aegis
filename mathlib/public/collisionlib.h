#pragma once

#include <Eigen/Dense>

namespace Mathlib
{
    typedef struct
    {
        bool hit;
        Eigen::Vector3f pos;
        Eigen::Vector3f normal;
    } hitresult_t;

    hitresult_t RayCuboid(Eigen::Vector3f o, Eigen::Vector3f r, Eigen::Vector3f cuboid, float cuboidradius);
}