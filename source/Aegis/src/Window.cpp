#include "Window.h"

#include "Input.h"

void Window::Present(void)
{
	glfwSwapBuffers(win);
	glfwPollEvents();
}

bool Window::ShouldClose(void)
{
	return glfwWindowShouldClose(win);
}

void Window::GetSize(void)
{
	glfwGetWindowSize(win, &w, &h);
}

bool Window::MakeWindow(int w, int h, std::string name)
{
	win = glfwCreateWindow(w, h, name.c_str(), NULL, NULL);
	glfwMakeContextCurrent(win);

	glfwSetKeyCallback(win, Input::KeyCallback);
	glfwSetCharCallback(win, Input::CharCallback);
	glfwSetCursorPosCallback(win, Input::CursorPosCallback);

	return win;
}