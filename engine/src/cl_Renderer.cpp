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
    renderer::Image *drawimg;

    frame = renderer.CurFrame();

    frame->renderfence.Wait(1000000000);
    frame->renderfence.Reset();
    
    iswapchain = renderer.SwapchainImage(1000000000, &frame->swapchainsem, NULL);
    img = &renderer.swapchainimgs[iswapchain];
    drawimg = &renderer.drawimg;

    frame->maincmdbuf.Begin(renderer::CmdBuf::USAGE_ONE_TIME_SUBMIT);

    drawimg->layout = renderer::Image::LAYOUT_UNDEFINED;
    drawimg->TransitionLayout(&frame->maincmdbuf, renderer::Image::LAYOUT_GENERAL);
    frame->maincmdbuf.CmdClearColorImage(drawimg, drawimg->layout, Eigen::Vector3f(1, 0, 0));
    drawimg->TransitionLayout(&frame->maincmdbuf, renderer::Image::LAYOUT_TRANSFER_SRC_OPTIMAL);
    
    img->layout = renderer::Image::LAYOUT_UNDEFINED;
    img->TransitionLayout(&frame->maincmdbuf, renderer::Image::LAYOUT_TRANSFER_DST_OPTIMAL);
    drawimg->BlitToImage(&frame->maincmdbuf, img);
    img->TransitionLayout(&frame->maincmdbuf, renderer::Image::LAYOUT_PRESENT_SRC);
    
    frame->maincmdbuf.End();

    renderer.gfxque.SubmitCmdBuf(&frame->maincmdbuf, 
        &frame->swapchainsem, &frame->rendersem, 
        &frame->renderfence, 
        renderer::STAGE_COLOR_ATTACHMENT_OUTPUT, renderer::STAGE_ALL_GRAPHICS);

    renderer.Present(iswapchain);
}

void engine::cl::Renderer::Init(void)
{
    renderer.Initialize();
}

void engine::cl::Renderer::Shutdown(void)
{
    renderer.Shutdown();
}