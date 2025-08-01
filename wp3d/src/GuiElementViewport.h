#pragma once

#include "GuiElement.h"

#include "Viewport.h"

class GuiElementViewport : public GuiElement
{
private:
    Map& map;
    Viewport view;

    uint64_t lastframe;

    void RegenFBO(void);
    void KeyboardInput(float deltatime);
    void MouseInput(void);
public:
    GuiElementViewport(Map& map, Viewport::viewporttype_e type);

    void Draw(void) override;
};