#pragma once

#include "BrFace.h"

class Brush
{
public:
    std::vector<BrFace> planes;
    Eigen::Vector3f bb[2];

    // used when culling interior faces. dont touch unless you know what you're doing
    std::vector<Mathlib::Poly<3>> interior;
    std::vector<Mathlib::Poly<3>> exterior;

    void Polygonize(void);
    void FindBB(void);
    void Expand(const Eigen::Vector3f hull[2]);
    // if priorty is set to true, otherbrush will get priority on coplanar faces.
    // assumes interior is already populated.
    void SeperateInOut(const Brush& otherbrush);
};