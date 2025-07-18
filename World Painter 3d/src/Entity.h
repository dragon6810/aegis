#pragma once

#include <string>
#include <unordered_map>

#include "Brush.h"

class Entity
{
public:
    std::unordered_map<std::string, std::string> pairs;
    std::vector<Brush> brushes;
};