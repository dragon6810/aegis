#pragma once

#include <string.h>

#include <Eigen/Dense>

#include "Polygon.h"
#include "Viewport.h"

class Plane
{
private:
    void DrawWire(const Viewport& view);
public:
    Eigen::Vector3f normal = Eigen::Vector3f(0, 0, 0);
    float d = 0;
    std::string texture = "";
    Polygon poly; // cached, must be updated when brush is updated

    void Draw(const Viewport& view);
};