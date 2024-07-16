#include "TextPanel.h"

std::shared_ptr<GuiWindow> TextPanel::GetWindow()
{
	return this->window;
}

void TextPanel::SetWindow(std::shared_ptr<GuiWindow> window)
{
	this->window = window;
}