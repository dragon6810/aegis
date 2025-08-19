#include "cl_Renderer.h"

engine::cl::Renderer::Renderer(Client& cl) : cl(cl)
{
    
}

engine::cl::Renderer::~Renderer(void)
{

}

void engine::cl::Renderer::Render(void)
{
    
}

void engine::cl::Renderer::Init(void)
{
    renderer.Initialize();
}

void engine::cl::Renderer::Shutdown(void)
{
    renderer.Shutdown();
}