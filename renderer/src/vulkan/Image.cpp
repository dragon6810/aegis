#include "ImplVulkan.h"

#include <stdio.h>

#include <utilslib.h>

void renderer::Image::TransitionLayout(CmdBuf* cmdbuf, layout_e dstlayout)
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
    imgbarrier.oldLayout = (VkImageLayout) layout;
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

    layout = dstlayout;
}

void renderer::Image::BlitToImage(CmdBuf* cmdbuf, Image* dst)
{
    VkImageBlit2 region;
    VkBlitImageInfo2 blitinfo;

    UTILS_ASSERT(renderer);

    UTILS_ASSERT(cmdbuf);
    UTILS_ASSERT(dst);

    region = {};
    region.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
	region.srcOffsets[1].x = size[0];
	region.srcOffsets[1].y = size[1];
	region.srcOffsets[1].z = 1;

	region.dstOffsets[1].x = dst->size[0];
	region.dstOffsets[1].y = dst->size[1];
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

void renderer::Image::Create(Eigen::Vector2i size, uint32_t usageflags, uint32_t aspectflags, Image::format_e fmt)
{
    VkResult res;
    VkImageCreateInfo imginfo;
    VmaAllocationCreateInfo allocinfo;
    VkImageViewCreateInfo viewinfo;

    this->size = size;

    impl->extent = { (uint32_t) size[0], (uint32_t) size[1], 1u, };
    format = fmt;

    imginfo = {};
    imginfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imginfo.imageType = VK_IMAGE_TYPE_2D;
    imginfo.format = (VkFormat) fmt;
    imginfo.extent = impl->extent;
    imginfo.mipLevels = 1;
    imginfo.arrayLayers = 1;
    imginfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imginfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imginfo.usage = usageflags;

    allocinfo = {};
    allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    res = vmaCreateImage(renderer->impl->allocator, &imginfo, &allocinfo, &impl->vkimg, &impl->allocation, NULL);
    if(res != VK_SUCCESS)
    {
        printf("error %d when allocating image!\n", res);
        exit(1);
    }

    viewinfo = {};
    viewinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewinfo.image = impl->vkimg;
    viewinfo.format = (VkFormat) format;
    viewinfo.subresourceRange.levelCount = 1;
    viewinfo.subresourceRange.baseArrayLayer = 0;
    viewinfo.subresourceRange.layerCount = 1;
    viewinfo.subresourceRange.aspectMask = aspectflags;

    res = vkCreateImageView(renderer->impl->device, &viewinfo, NULL, &impl->imgview);
    if(res != VK_SUCCESS)
    {
        printf("error %d when creating image view!\n", res);
        exit(1);
    }

    printf("created view 0x%016llx\n", impl->imgview);
}

void renderer::Image::Init(Renderer* renderer)
{
    UTILS_ASSERT(renderer);

    this->renderer = renderer;
    this->impl = std::make_unique<Impl>();
}

void renderer::Image::Shutdown(void)
{
    if(impl->imgview)
    {
        vkDestroyImageView(renderer->impl->device, impl->imgview, NULL);
        impl->imgview = NULL;
    }

    if(impl->vkimg)
    {
        if(impl->allocation)
            vmaDestroyImage(renderer->impl->allocator, impl->vkimg, impl->allocation);
        else
            vkDestroyImage(renderer->impl->device, impl->vkimg, NULL);

        impl->vkimg = NULL;
        impl->allocation = NULL;
    }
}
