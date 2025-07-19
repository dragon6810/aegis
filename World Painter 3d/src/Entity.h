#pragma once

#include <string>
#include <unordered_map>

#include "Brush.h"
#include "Viewport.h"

class Entity
{
public:
    std::unordered_map<std::string, std::string> pairs;
    std::vector<Brush> brushes;
    
    void Draw(const Viewport& view);
};