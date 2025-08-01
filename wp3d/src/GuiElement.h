#pragma once

#include "Map.h"

class GuiElement
{
private:
    Map& map;
public:
    GuiElement(Map& map);

    virtual void Draw(void);
};