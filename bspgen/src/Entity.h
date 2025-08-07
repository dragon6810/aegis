#pragma once

#include <map>

#include "Brush.h"

class Entity
{
public:
    std::map<std::string, std::string> pairs;
    std::vector<Brush> brushes;
};