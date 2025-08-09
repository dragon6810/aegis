#pragma once

#include <Eigen/Dense>

class BspPlane
{
public:
    Eigen::Vector3f n;
    float d;
    int nodenum = -1;
};