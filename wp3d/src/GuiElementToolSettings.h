#pragma once

#include "GuiElement.h"

class GuiElementToolSettings : public GuiElement
{
private:
    Map& map;

    void SettingsSelect(void);
    void SettingsEntity(void);
public:
    GuiElementToolSettings(Map& map);

    void Draw(void) override;
};