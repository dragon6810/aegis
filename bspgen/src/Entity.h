#pragma once

#include <map>

#include <bsplib.h>

#include "Brush.h"
#include "BspTree.h"
#include "HullDef.h"

class BspBuilder;

class Entity
{
private:
    BspBuilder& builder;
public:
    Entity(BspBuilder& builder);

    std::map<std::string, std::string> pairs;
    std::vector<Brush> brushes[Bsplib::n_hulls];
    BspTree bsp[Bsplib::n_hulls];

    void ExpandBrushes(const HullDef& hulls);
    void CullInterior(void);
};