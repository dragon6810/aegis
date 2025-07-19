#pragma once

#include <vector>

#include "Plane.h"
#include "Viewport.h"

class Brush
{
public:
    std::vector<Plane> planes;

    void MakeFaces(void);
    void Draw(const Viewport& view);
};
