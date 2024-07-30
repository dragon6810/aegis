#include "TextPanel.h"

#include "Game.h"

#if 0
GuiWindow* TextPanel::GetWindow()
{
	return this->window;
}

void TextPanel::MouseDrag(int x, int y)
{
	
}

void TextPanel::MouseDown(int x, int y)
{
	mousedown = true;

	int realx = this->x + window->x;
	int realy = this->y + window->y - (Game::GetGame().gui.windowborderh << 1);
}

void TextPanel::MouseUp(int x, int y)
{
	mousedown = false;
}

void TextPanel::SetWindow(GuiWindow* window)
{
	this->window = window;
}
#endif