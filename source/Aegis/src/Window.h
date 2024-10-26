#pragma once

#include <string>

#include <GLFW/glfw3.h>

class Window
{
public:
	int w, h; // These could be out of date, call GetSize()

	bool MakeWindow(int w, int h, std::string name);

	void GetSize();
	bool ShouldClose();

	void Present();
private:
	GLFWwindow* win;
};