#include "ImplVulkan.h"

#include <stdio.h>

#include <utilslib.h>

void renderer::CmdBuf::Impl::Alloc(VkCommandPool pool)
{
    VkResult res;
    VkCommandBufferAllocateInfo cmdbufinfo;

    cmdbufinfo = {};
    cmdbufinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdbufinfo.pNext = nullptr;
    cmdbufinfo.commandPool = pool;
    cmdbufinfo.commandBufferCount = 1;
    cmdbufinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    res = vkAllocateCommandBuffers(buf->renderer->impl->device, &cmdbufinfo, &cmdbuf);
    if(res != VK_SUCCESS)
    {
        printf("error %d when allocating vulkan cmd buf!\n", res);
        exit(1);
    }
}

void renderer::CmdBuf::Reset(bool releaseresources)
{
    VkResult res;
    VkCommandBufferResetFlags flags;

    flags = 0;
    if(releaseresources)
        flags |= VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT;

    res = vkResetCommandBuffer(impl->cmdbuf, flags);
    if(res != VK_SUCCESS)
    {
        printf("error %d with vkResetCommandBuffer!\n", res);
        exit(1);
    }
}

void renderer::CmdBuf::Begin(uint32_t usageflags)
{
    VkResult res;
    VkCommandBufferBeginInfo begininfo;

    begininfo = {};
    begininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begininfo.pInheritanceInfo = NULL;
    begininfo.flags = usageflags;

    res = vkBeginCommandBuffer(impl->cmdbuf, &begininfo);
    if(res != VK_SUCCESS)
    {
        printf("error %d with vkBeginCommandBuffer!\n", res);
        exit(1);
    }
}

void renderer::CmdBuf::End(void)
{
    VkResult res;

    res = vkEndCommandBuffer(impl->cmdbuf);
    if(res != VK_SUCCESS)
    {
        printf("error %d with vkEndCommandBuffer!\n", res);
        exit(1);
    }
}

void renderer::CmdBuf::Init(Frame* frame, Renderer* renderer)
{
    this->renderer = renderer;
    impl = std::make_unique<Impl>();
    impl->buf = this;
}

void renderer::CmdBuf::Shutdown(void)
{

}
