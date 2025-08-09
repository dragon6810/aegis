#pragma once

#include <vector>

class BspNode
{
public:
    int plnum = -1;
    std::vector<int> faces;
    int children[2]; // if negative, bitwise inverse into leaves
};