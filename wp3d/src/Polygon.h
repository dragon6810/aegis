#pragma once

#include <Eigen/Dense>

class Polygon
{
public:
    std::vector<Eigen::Vector3f> points;

    void Clear(void);
    void FromPlane(Eigen::Vector3f n, float d, float radius);
    // 0 is behind, else is front
    void Clip(Eigen::Vector3f n, float d, int side);
};