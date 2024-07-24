#pragma once

#include <memory>

#include "GuiWindow.h"

class TextPanel
{
public:
	GuiWindow* GetWindow();

	void MouseDrag(int x, int y);
	void MouseDown(int x, int y);
	void MouseUp(int x, int y);

	void SetWindow(GuiWindow* window);

	int x, y;
	int w, h;

	void Draw();
protected:
	GuiWindow* window;

	int textpad = 3;
};

