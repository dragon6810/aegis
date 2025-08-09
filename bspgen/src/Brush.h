#pragma once

#include "BrFace.h"

class Brush
{
public:
    std::vector<BrFace> planes;
    Eigen::Vector3f bb[2];

    // used when culling interior faces. dont touch unless you know what you're doing
    std::vector<BrFace> interior;
    std::vector<BrFace> exterior;

    void Polygonize(void);
    void FindBB(void);
    void Expand(const Eigen::Vector3f hull[2]);
    // assumes up-to-date bbs on both brushes
    bool Overlaps(const Brush& otherbrush);
    // assumes up-to-date polygonization
    void PopulateExterior(void);
    // if priorty is set to true, otherbrush will get priority on coplanar faces.
    // assumes exterior is already populated.
    void SeperateInOut(const Brush& otherbrush, bool priority);
};