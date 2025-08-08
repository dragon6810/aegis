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

    void ExpandBrushes(const HullDef& hulls);
};