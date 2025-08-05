#pragma once

#include "GuiElement.h"

#include <GL/glew.h>

#include <tpklib.h>

class GuiElementTextureBrowser : public GuiElement
{
private:
    static constexpr float pad = 4.0;

    Map& map;

    int selected = -1;
    ImGuiTextFilter filter;

    void DrawTex(TextureManager::texture_t* tex, float width, int id, int column, int row, int ncolumns);
public:
    GuiElementTextureBrowser(Map& map);

    void Draw(void) override;
};