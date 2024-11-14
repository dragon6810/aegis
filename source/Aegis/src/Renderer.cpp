#include "Renderer.h"

void Renderer::Submit()
{
	window->Present();
}

void Renderer::Clear()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	window->GetSize();

	#ifdef __APPLE__
		glViewport(0, 0, window->w<<1, window->h<<1);
	#else
		glViewport(0, 0, window->w, window->h);
	#endif
}

bool Renderer::PostWindow(Window* window)
{
	this->window = window;

	glewInit();
	//glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	return true;
}

bool Renderer::PreWindow()
{
	if (!glfwInit())
		return false;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	return true;
}