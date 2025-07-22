#pragma once

#include "ResourceManager.h"

class RasterFont
{
public:
    ResourceManager::texture_t* tex;
    short offsets[256];
    short widths[256];

    int DrawText(std::string name, int x, int y);
private:
    int DrawChar(char c, int x, int y);
};