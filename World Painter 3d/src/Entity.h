#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Brush.h"
#include "Viewport.h"

class Entity
{
public:
    std::unordered_map<std::string, std::string> pairs;
    std::vector<Brush> brushes;
    std::unordered_set<int> brselection;
    
    void Draw(const Viewport& view, int index, const Map& map);
};