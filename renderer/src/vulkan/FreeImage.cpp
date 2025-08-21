#include "ImplVulkan.h"

#include <stdio.h>

#include <utilslib.h>

void renderer::FreeImage::TransitionLayout(CmdBuf* cmdbuf, Image::layout_e dstlayout)
{
    img.TransitionLayout(cmdbuf, layout, dstlayout);
    layout = dstlayout;
}

void renderer::FreeImage::BlitToImage(CmdBuf* cmdbuf, Image* dst, Eigen::Vector2i dstsize)
{
    img.BlitToImage(cmdbuf, dst, size, dstsize);
}

void renderer::FreeImage::Init(Eigen::Vector2i size, uint32_t usageflags, uint32_t aspectflags, Image::format_e fmt, Renderer* renderer)
{
    VkResult res;
    VkImageCreateInfo imginfo;
    VmaAllocationCreateInfo allocinfo;
    VkImageViewCreateInfo viewinfo;

    this->renderer = renderer;
    impl = std::make_unique<Impl>();
    img.Init(renderer);

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

    res = vmaCreateImage(renderer->impl->allocator, &imginfo, &allocinfo, &img.impl->vkimg, &impl->allocation, NULL);
    if(res != VK_SUCCESS)
    {
        printf("error %d when allocating image!\n", res);
        exit(1);
    }

    viewinfo = {};
    viewinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewinfo.image = img.impl->vkimg;
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
}

void renderer::FreeImage::Destroy(void)
{
    UTILS_ASSERT(renderer);

    vkDestroyImageView(renderer->impl->device, impl->imgview, nullptr);
	vmaDestroyImage(renderer->impl->allocator, img.impl->vkimg, impl->allocation);
}