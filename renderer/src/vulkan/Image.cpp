#include "ImplVulkan.h"

#include <stdio.h>

#include <utilslib.h>

void renderer::Image::TransitionLayout(CmdBuf* cmdbuf, layout_e srclayout, layout_e dstlayout)
{
    VkImageMemoryBarrier2KHR imgbarrier;
    VkDependencyInfo depinfo;

    UTILS_ASSERT(cmdbuf);
    
    imgbarrier = {};
    imgbarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR;
    imgbarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imgbarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    imgbarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imgbarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
    imgbarrier.oldLayout = (VkImageLayout) srclayout;
    imgbarrier.newLayout = (VkImageLayout) dstlayout;
    
    imgbarrier.subresourceRange = {};
    imgbarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    if(dstlayout == LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
        imgbarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imgbarrier.subresourceRange.baseMipLevel = 0;
    imgbarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    imgbarrier.subresourceRange.baseArrayLayer = 0;
    imgbarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    imgbarrier.image = impl->vkimg;

    depinfo = {};
    depinfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depinfo.imageMemoryBarrierCount = 1;
    depinfo.pImageMemoryBarriers = &imgbarrier;

    renderer->impl->pipelinebarrier2proc(cmdbuf->impl->cmdbuf, &depinfo);
}

void renderer::Image::Init(Renderer* renderer)
{
    UTILS_ASSERT(renderer);

    this->renderer = renderer;
    this->impl = std::make_unique<Impl>();
}

void renderer::Image::Shutdown(void)
{

}
