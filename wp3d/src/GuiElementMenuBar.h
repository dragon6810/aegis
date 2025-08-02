#pragma once

#include "GuiElement.h"

#include "GuiElementConfigMenu.h"

class GuiElementMenuBar : public GuiElement
{
private:
    Map& map;

    GuiElementConfigMenu cfgmenu;

    void DrawMenuFile(void);
    void DrawMenuEdit(void);
    void DrawFileDialogs(void);

    void New(void);
    void Open(void);
    void Save(void);
    void SaveAs(void);
    void Options(void);
public:
    GuiElementMenuBar(Map& map);

    void Draw(void) override;
};