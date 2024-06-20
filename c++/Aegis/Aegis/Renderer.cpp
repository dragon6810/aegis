#include "Renderer.h"

#include "OpenGL.h"

void Renderer::Init()
{
	OpenGL::GLInit();
}

void Renderer::PostWindowInit()
{
	OpenGL::PostWindowInit();
}