#pragma once

#include "Window.h"

class Renderer
{
public:
	bool PreWindow(void);
	bool PostWindow(Window* window);

	void Clear(void);
	void Submit(void);
private:
	Window* window;
};