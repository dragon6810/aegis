#pragma once

#include "GuiElement.h"

class GuiElementToolBar : public GuiElement
{
private:
    Map& map;

    void DrawToolButton(const char* label, Map::tooltype_e type, const char* shortcut, const char* desc);
public:
    GuiElementToolBar(Map& map);

    void Draw(void) override;
};