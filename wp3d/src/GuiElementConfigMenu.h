#pragma once

#include "GuiElement.h"

class GuiElementConfigMenu : public GuiElement
{
private:
    Map& map;

    bool showmenu;
    Cfglib::CfgFile workingcfg;
public:
    GuiElementConfigMenu(Map& map);

    void Show(void);

    void Draw(void) override;
};