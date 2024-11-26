#pragma once

#include <string>

#include "ResourceManager.h"
#include "RasterFont.h"

class Wad
{
public:
    ~Wad(void);

    void Open(std::string filename);
    void Unload(void);

    ResourceManager::texture_t* LoadTexture(std::string name);
    RasterFont LoadFont(std::string name);
private:
    FILE* ptr = 0;
};