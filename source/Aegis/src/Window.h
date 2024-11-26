#pragma once

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	int w, h; // These could be out of date, call GetSize()

	bool MakeWindow(int w, int h, std::string name);

	void GetSize(void);
	bool ShouldClose(void);

	void Present(void);
private:
	GLFWwindow* win;
};