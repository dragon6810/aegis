#pragma once

#include <memory>

#include "GuiWindow.h"

class TextPanel
{
public:
	std::shared_ptr<GuiWindow> GetWindow();

	void SetWindow(std::shared_ptr<GuiWindow> window);
protected:
	std::shared_ptr<GuiWindow> window;

	int textpad = 3;
};

