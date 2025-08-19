#include "cl_Renderer.h"

#include <utilslib.h>

engine::cl::Renderer::Renderer(Client& cl) : cl(cl)
{
    
}

engine::cl::Renderer::~Renderer(void)
{

}

void engine::cl::Renderer::Render(void)
{
    renderer::Frame *frame;
    renderer::Image *img;

    frame = renderer.CurFrame();

    frame->renderfence.Wait(1000000000);
    frame->renderfence.Reset();
    
    iswapchain = renderer.SwapchainImage(1000000000, &frame->swapchainsem, NULL);
    
    frame->maincmdbuf.Begin(renderer::CmdBuf::USAGE_ONE_TIME_SUBMIT);
    img = &renderer.swapchainimgs[iswapchain];
    img->TransitionLayout(&frame->maincmdbuf, renderer::Image::LAYOUT_UNDEFINED, renderer::Image::LAYOUT_GENERAL);
    frame->maincmdbuf.CmdClearColorImage(img, renderer::Image::LAYOUT_GENERAL, Eigen::Vector3f::UnitX());
    img->TransitionLayout(&frame->maincmdbuf, renderer::Image::LAYOUT_GENERAL, renderer::Image::LAYOUT_PRESENT_SRC);
    frame->maincmdbuf.End();
}

void engine::cl::Renderer::Init(void)
{
    renderer.Initialize();
}

void engine::cl::Renderer::Shutdown(void)
{
    renderer.Shutdown();
}