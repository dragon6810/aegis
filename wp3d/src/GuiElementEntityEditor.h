#pragma once

#include "GuiElement.h"

class GuiElementEntityEditor : public GuiElement
{
private:
    Map& map;

    bool smartedit = true;
    int selectedpair = -1;

    void DrawPairs(void);
    void DrawHelper(void);
    void DrawPairsSmartEdit(Entity* ent);
    void DrawPairsRaw(Entity* ent);
    void DrawClassDropdown(Entity* ent, int idef);
    void SwitchClass(Entity* ent, Fgdlib::EntityDef* newclass);
    void DrawSmartPair(Entity* ent, int ipair, Fgdlib::EntityPair* pair);
    void DrawPairPopup(Entity* ent);
public:
    GuiElementEntityEditor(Map& map);

    void Draw(void) override;
};