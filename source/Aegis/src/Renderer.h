#pragma once

#include "Window.h"

class Renderer
{
public:
	bool PreWindow();
	bool PostWindow(Window* window);

	void Clear();
	void Submit();
private:
	Window* window;
};