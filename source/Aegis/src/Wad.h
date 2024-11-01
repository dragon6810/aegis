#pragma once

#include <string>

#include "ResourceManager.h"
#include "RasterFont.h"

class Wad
{
public:
    void Open(std::string filename);
    void Unload();

    ResourceManager::texture_t* LoadTexture(std::string name);
    RasterFont LoadFont(std::string name);
private:
    FILE* ptr = 0;
};