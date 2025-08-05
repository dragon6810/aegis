#pragma once

#include "GuiElement.h"

#include <GL/glew.h>

#include <tpklib.h>

class GuiElementTextureBrowser : public GuiElement
{
private:
    static constexpr float pad = 4.0;

    Map& map;

    Tpklib::TpkFile tpk;
    std::map<std::string, GLuint> gltex;
    std::map<std::string, std::string> dirnames;
    int selected = -1;
    ImGuiTextFilter filter;

    void GenTextures(void);
    void ReloadTpk(void);
    void DrawTex(Tpklib::TpkTex* tex, GLuint glid, float width, int id, int column, int row, int ncolumns);
public:
    GuiElementTextureBrowser(Map& map);

    void Draw(void) override;
};