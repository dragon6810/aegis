#pragma once

#include "GuiElement.h"

#include <GL/glew.h>

#include <tpklib.h>

class GuiElementTextureBrowser : public GuiElement
{
private:
    Map& map;

    Tpklib::TpkFile tpk;
    std::map<std::string, GLuint> gltex;
    int selected = -1;

    void GenTextures(void);
    void ReloadTpk(void);
    void DrawTex(Tpklib::TpkTex* tex, GLuint glid, float width, int id);
public:
    GuiElementTextureBrowser(Map& map);

    void Draw(void) override;
};