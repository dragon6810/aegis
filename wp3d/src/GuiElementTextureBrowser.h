#pragma once

#include "GuiElement.h"

#include <GL/glew.h>

#include <tpklib.h>

class GuiElementTextureBrowser : public GuiElement
{
private:
    Map& map;

    Tpklib::TpkFile tpk;
    std::unordered_map<std::string, GLuint> gltex;

    void GenTextures(void);
    void ReloadTpk(void);
    void DrawTex(Tpklib::TpkTex* tex, GLuint glid);
public:
    GuiElementTextureBrowser(Map& map);

    void Draw(void) override;
};