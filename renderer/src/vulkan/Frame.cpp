#include "ImplVulkan.h"

#include <stdio.h>

#include <utilslib.h>

void renderer::Frame::Impl::ShutdownCmdPools(void)
{
    vkDestroyCommandPool(frame->renderer->impl->device, cmdpool, NULL);
}

void renderer::Frame::Impl::ShutdownSync(void)
{
    frame->rendersem.Shutdown();
    frame->swapchainsem.Shutdown();
    frame->renderfence.Shutdown();
}

void renderer::Frame::Impl::InitSync(void)
{
    frame->renderfence.Init(true, frame->renderer);
    frame->swapchainsem.Init(frame->renderer);
    frame->rendersem.Init(frame->renderer);
}

void renderer::Frame::Impl::InitCmdPools(void)
{
    VkResult res;
    VkCommandPoolCreateInfo cmdpoolinfo;
    
    cmdpoolinfo = {};
    cmdpoolinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdpoolinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	cmdpoolinfo.queueFamilyIndex = frame->renderer->gfxque.impl->quefam;

    res = vkCreateCommandPool(frame->renderer->impl->device, &cmdpoolinfo, NULL, &cmdpool);
    if(res != VK_SUCCESS)
    {
        printf("error %d when creating vulkan command pool!\n", res);
        exit(1);
    }

    frame->maincmdbuf.impl->Alloc(cmdpool);
}

void renderer::Frame::Impl::Init(void)
{
    InitCmdPools();
    InitSync();
}

void renderer::Frame::Impl::Shutdown(void)
{
    ShutdownSync();
    ShutdownCmdPools();
}

void renderer::Frame::Init(Renderer* renderer)
{
    UTILS_ASSERT(renderer);

    this->renderer = renderer;

    impl = std::make_unique<Impl>();
    impl->frame = this;

    maincmdbuf.Init(this, renderer);
    impl->Init();
}

void renderer::Frame::Shutdown(void)
{
    UTILS_ASSERT(renderer);

    impl->Shutdown();
}
