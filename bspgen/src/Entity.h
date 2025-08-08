#pragma once

#include <map>

#include <bsplib.h>

#include "Brush.h"
#include "HullDef.h"

class Entity
{
public:
    std::map<std::string, std::string> pairs;
    std::vector<Brush> brushes[Bsplib::n_hulls];
    std::vector<Mathlib::Poly<3>> geometry[Bsplib::n_hulls]; // post-csg

    void ExpandBrushes(const HullDef& hulls);
    void CullInterior(void);
};