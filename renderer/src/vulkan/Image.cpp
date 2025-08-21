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

void renderer::Image::BlitToImage(CmdBuf* cmdbuf, Image* dst, Eigen::Vector2i srcsize, Eigen::Vector2i dstsize)
{
    VkImageBlit2 region;
    VkBlitImageInfo2 blitinfo;

    UTILS_ASSERT(renderer);

    UTILS_ASSERT(cmdbuf);
    UTILS_ASSERT(dst);

    region = {};
    region.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
	region.srcOffsets[1].x = srcsize[0];
	region.srcOffsets[1].y = srcsize[1];
	region.srcOffsets[1].z = 1;

	region.dstOffsets[1].x = dstsize[0];
	region.dstOffsets[1].y = dstsize[1];
	region.dstOffsets[1].z = 1;

	region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.srcSubresource.baseArrayLayer = 0;
	region.srcSubresource.layerCount = 1;
	region.srcSubresource.mipLevel = 0;

	region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.dstSubresource.baseArrayLayer = 0;
	region.dstSubresource.layerCount = 1;
	region.dstSubresource.mipLevel = 0;

	blitinfo = {};
    blitinfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
	blitinfo.dstImage = dst->impl->vkimg;
	blitinfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitinfo.srcImage = impl->vkimg;
	blitinfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitinfo.filter = VK_FILTER_LINEAR;
	blitinfo.regionCount = 1;
	blitinfo.pRegions = &region;

	renderer->impl->blitimage2proc(cmdbuf->impl->cmdbuf, &blitinfo);
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
