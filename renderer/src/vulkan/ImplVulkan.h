#pragma once

#include <renderer/Renderer.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <VkBootstrap.h>

struct renderer::Renderer::Impl
{
    renderer::Renderer *renderer = NULL;

    vkb::Instance vkbinst;
    VkInstance inst;
	VkDebugUtilsMessengerEXT dbgmessenger;
    vkb::PhysicalDevice vkbphysicaldevice;
	VkPhysicalDevice physicaldevice;
    vkb::Device vkbdevice;
	VkDevice device;
    VmaAllocator allocator;
	VkSurfaceKHR surface;
    SDL_Window *win = NULL;

    VkSwapchainKHR swapchain;
	VkFormat swapchainformat;
	std::vector<VkImageView> swapchainviews;
	VkExtent2D swapchainextent;

    PFN_vkCmdPipelineBarrier2KHR pipelinebarrier2proc;
    PFN_vkQueueSubmit2KHR queuesubmit2proc;
    PFN_vkCmdBlitImage2KHR blitimage2proc;

    void VkShutdownInst(void);
    void VkShutdownDevice(void);
    void VkShutdownVma(void);
    void VkShutdownSwapchain(void);
    void VkShutdown(void);
    void SDLShutdown(void);

    void VkInitQues(void);
    void VkInitSwapchain(void);
    void VkInitVma(void);
    void VkInitDevice(void);
    void VkInitInst(void);
    void VkInit(void);
    void SDLInit(void);

    void Initialize(void);
    void Shutdown(void);
};

struct renderer::Frame::Impl
{
    renderer::Frame *frame = NULL;

    VkCommandPool cmdpool;

    void ShutdownCmdPools(void);
    void ShutdownSync(void);

    void InitSync(void);
    void InitCmdPools(void);

    void Init(void);
    void Shutdown(void);
};

struct renderer::Queue::Impl
{
    VkQueue vkque;
    uint32_t quefam;
};

struct renderer::CmdBuf::Impl
{
    renderer::CmdBuf *buf = NULL;

    VkCommandBuffer cmdbuf;

    void Alloc(VkCommandPool pool);
};

struct renderer::Semaphore::Impl
{
    Semaphore *sem = NULL;

    VkSemaphore vksem;

    void Init(void);
    void Shutdown(void);
};

struct renderer::Fence::Impl
{
    Fence *fence = NULL;

    VkFence vkfence;

    void Reset(void);
    void Wait(uint64_t timeoutns);

    void Init(bool startsignaled);
    void Shutdown(void);
};

struct renderer::FreeImage::Impl
{
    VkImageView imgview;
    VmaAllocation allocation;
    VkExtent3D extent;
};

struct renderer::Image::Impl
{
    VkImage vkimg;
};