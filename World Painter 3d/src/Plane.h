#pragma once

#include <string.h>

#include <Eigen/Dense>

class Plane
{
public:
    Eigen::Vector3f normal = Eigen::Vector3f(0, 0, 0);
    float d = 0;
    std::string texture = "";
};