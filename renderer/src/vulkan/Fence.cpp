#include "ImplVulkan.h"

#include <stdio.h>

#include <utilslib.h>

void renderer::Fence::Impl::Reset(void)
{
    vkResetFences(fence->renderer->impl->device, 1, &vkfence);
}

void renderer::Fence::Impl::Wait(uint64_t timeoutns)
{
    vkWaitForFences(fence->renderer->impl->device, 1, &vkfence, true, timeoutns);
}

void renderer::Fence::Impl::Init(bool startsignaled)
{
    VkResult res;
    VkFenceCreateInfo fencecreateinfo;

    fencecreateinfo = {};
    fencecreateinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if(startsignaled)
        fencecreateinfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    res = vkCreateFence(fence->renderer->impl->device, &fencecreateinfo, NULL, &vkfence);
    if(res != VK_SUCCESS)
    {
        printf("error %d when creating vulkan fence!\n", res);
        exit(1);
    }
}

void renderer::Fence::Impl::Shutdown(void)
{
    vkDestroyFence(fence->renderer->impl->device, vkfence, NULL);
}

void renderer::Fence::Reset(void)
{
    UTILS_ASSERT(renderer);

    impl->Reset();
}

void renderer::Fence::Wait(uint64_t timeoutns)
{
    UTILS_ASSERT(renderer);

    impl->Wait(timeoutns);
}

void renderer::Fence::Init(bool startsignaled, Renderer* renderer)
{
    UTILS_ASSERT(renderer);

    this->renderer = renderer;
    impl = std::make_unique<Impl>();
    impl->fence = this;

    impl->Init(startsignaled);
}

void renderer::Fence::Shutdown(void)
{
    UTILS_ASSERT(renderer);

    impl->Shutdown();
}
