#pragma once

#include "GLFW/glfw3.h"

#include <string>

#include "defs.h";

class Window
{
public:
	// Call with width or height 0 to make fullscreen windowed monitor
	Window(std::string name, int width, int height, bool fullscreen);
	Window();
	~Window();
	void Kill();

	void SelectForRendering();
	void MakeFullscreenViewport(float aspect);
	void SetKeyCallback(GLFWkeyfun func);
	void SetCursorPosCallback(GLFWcursorposfun func);
	void SetMouseBtnCallback(GLFWmousebuttonfun func);

	void SetCursor(int shape);
	
	void GetWindowDimensions(int* x, int* y);
	float GetWindowAspect();

	bool ShouldWindowClose();
	void SwapBuffers();
private:
	GLFWwindow* window;
};

