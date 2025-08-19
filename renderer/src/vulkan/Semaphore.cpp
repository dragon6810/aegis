#include "ImplVulkan.h"

#include <stdio.h>

#include <utilslib.h>

void renderer::Semaphore::Impl::Init(void)
{
    VkResult res;
    VkSemaphoreCreateInfo semcreateinfo;

    semcreateinfo = {};
    semcreateinfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semcreateinfo.pNext = nullptr;
    semcreateinfo.flags = 0;

    res = vkCreateSemaphore(sem->renderer->impl->device, &semcreateinfo, NULL, &vksem);
    if(res != VK_SUCCESS)
    {
        printf("error %d when creating vulkan semaphore!\n", res);
        exit(1);
    }
}

void renderer::Semaphore::Impl::Shutdown(void)
{
    vkDestroySemaphore(sem->renderer->impl->device, vksem, NULL);
}

void renderer::Semaphore::Init(Renderer* renderer)
{
    UTILS_ASSERT(renderer);

    this->renderer = renderer;
    impl = std::make_unique<Impl>();
    impl->sem = this;

    impl->Init();
}

void renderer::Semaphore::Shutdown(void)
{
    impl->Shutdown();
}
