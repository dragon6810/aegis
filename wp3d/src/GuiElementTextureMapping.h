#pragma once

#include "GuiElement.h"

#include <vector>

class GuiElementTextureMapping : public GuiElement
{
private:
    Map& map;

    std::vector<Plane*> GetSelectedFaces(void);
    void AlignGrid(void);
    void AlignFace(void);
    void Shift(Eigen::Vector2i shift);
    void Scale(Eigen::Vector2f scale);
    void Rotate(float degrees);

    void DrawQuickActions(void);
public:
    GuiElementTextureMapping(Map& map);

    void Draw(void) override;
};