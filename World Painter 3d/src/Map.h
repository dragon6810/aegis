#pragma once

#include "Entity.h"
#include "Viewport.h"

class Map
{
private:
    void SetupFrame(const Viewport& view);

public:
    static constexpr float max_map_size = 8192.0f; // Maximum size of the map in any direction

    std::vector<Entity> entities;

    void Render(const Viewport& view);
};