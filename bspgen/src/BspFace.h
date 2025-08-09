#pragma once

#include <mathlib.h>

#include "BrFace.h"

class BspBuilder;

class BspFace
{
public:
    BspFace(void);
    BspFace(const BrFace& brface, BspBuilder* builder);

    Mathlib::Poly<3> poly;
    int planenum = -1;
    bool flipplane;
    int texinfonum = -1;
};