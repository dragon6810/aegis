#include "Window.h"

#include "Input.h"

void Window::Present()
{
	glfwSwapBuffers(win);
	glfwPollEvents();
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(win);
}

void Window::GetSize()
{
	glfwGetWindowSize(win, &w, &h);
}

bool Window::MakeWindow(int w, int h, std::string name)
{
	win = glfwCreateWindow(w, h, name.c_str(), NULL, NULL);
	glfwMakeContextCurrent(win);

	glfwSetKeyCallback(win, Input::KeyCallback);

	return win;
}