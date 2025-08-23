#include "ImplVulkan.h"

#include <stdio.h>

#include <utilslib.h>

void renderer::Renderer::Impl::VkShutdownInst(void)
{
    vkb::destroy_debug_utils_messenger(inst, dbgmessenger);
	vkDestroyInstance(inst, nullptr);
}

void renderer::Renderer::Impl::VkShutdownDevice(void)
{
    vkDestroyDevice(device, NULL);
    vkDestroySurfaceKHR(inst, surface, NULL);
}

void renderer::Renderer::Impl::VkShutdownVma(void)
{
    vmaDestroyAllocator(allocator);
}

void renderer::Renderer::Impl::VkShutdownSwapchain(void)
{
    int i;

    renderer->drawimg.Shutdown();

    vkDestroySwapchainKHR(device, swapchain, NULL);
    for(i=0; i<renderer->swapchainimgs.size(); i++)
        vkDestroyImageView(device, renderer->swapchainimgs[i].impl->imgview, NULL);
}

void renderer::Renderer::Impl::VkShutdown(void)
{
    int i;

    vkDeviceWaitIdle(device);

    // VkShutdownInst(); // validation layer test

    for(i=0; i<max_fif; i++)
        renderer->frames[i].Shutdown();

    VkShutdownSwapchain();
    VkShutdownVma();
    VkShutdownDevice();
    VkShutdownInst();
}

void renderer::Renderer::Impl::SDLShutdown(void)
{
    SDL_DestroyWindow(win);
}

void renderer::Renderer::Impl::VkInitQues(void)
{
    renderer->gfxque.Init(Queue::TYPE_GFX, renderer);
}

void renderer::Renderer::Impl::VkInitSwapchain(void)
{
    int i;

    int w, h;
    VkSurfaceFormatKHR surfaceformat;
    vkb::SwapchainBuilder swapchainbuilder(physicaldevice, device, surface);
    vkb::Swapchain vkbswapchain;

    UTILS_ASSERT(SDL_GetWindowSize(win, &w, &h));

	swapchainformat = VK_FORMAT_B8G8R8A8_UNORM;
    
    surfaceformat = {};
    surfaceformat.format = swapchainformat;
    surfaceformat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    swapchainbuilder.set_desired_format(surfaceformat);
    // TODO: use mailbox instead at somepoint, it's supposed to be better.
    swapchainbuilder.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR);
    swapchainbuilder.set_desired_extent(w, h);
    swapchainbuilder.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT);

    vkb::Result<vkb::Swapchain> swapchainres = swapchainbuilder.build();
    if(!swapchainres)
    {
        printf("error %d when trying to create vulkan swapchain!\n", swapchainres.vk_result());
        exit(1);
    }

    vkbswapchain = swapchainres.value();
    swapchain = vkbswapchain.swapchain;
    
    renderer->swapchainimgs.resize(vkbswapchain.get_images().value().size());
    for(i=0; i<renderer->swapchainimgs.size(); i++)
    {
        renderer->swapchainimgs[i].Init(renderer);
        renderer->swapchainimgs[i].size = Eigen::Vector2i(w, h);
        renderer->swapchainimgs[i].format = (renderer::Image::format_e) swapchainformat;
        renderer->swapchainimgs[i].impl->vkimg = vkbswapchain.get_images().value()[i];
        renderer->swapchainimgs[i].impl->imgview = vkbswapchain.get_image_views().value()[i];
    }

    renderer->drawimg.Init(renderer);
    renderer->drawimg.Create
    (
        Eigen::Vector2i(w, h),
        Image::USAGE_TRANSFER_SRC | Image::USAGE_TRANSFER_DST | 
        Image::USAGE_STORAGE | Image::USAGE_COLOR_ATTACHMENT,
        Image::ASPECT_COLOR_BIT,
        Image::FORMAT_R16G16B16A16_SFLOAT
    );

    printf("vulkan/SDL3 swapchain created.\n");
}

void renderer::Renderer::Impl::VkInitVma(void)
{
    VkResult res;
    VmaAllocatorCreateInfo allocatorinfo;

    allocatorinfo = {};
    allocatorinfo.physicalDevice = physicaldevice;
    allocatorinfo.device = device;
    allocatorinfo.instance = inst;
    allocatorinfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    
    res = vmaCreateAllocator(&allocatorinfo, &allocator);
    if(res != VK_SUCCESS)
    {
        printf("error %d creating vulkan memory allocator!\n", res);
        exit(1);
    }

    printf("vulkan memory allocator created.\n");
}

void renderer::Renderer::Impl::VkInitDevice(void)
{
    VkPhysicalDeviceVulkan13Features device13features;
    VkPhysicalDeviceVulkan12Features device12features;
    VkPhysicalDeviceSynchronization2Features sync2features;
    vkb::PhysicalDeviceSelector deviceselector(vkbinst);

    if(!SDL_Vulkan_CreateSurface(win, inst, NULL, &surface))
    {
        printf("error creating SDL vulkan surface:\n%s\n", SDL_GetError());
        exit(1);
    }

    device13features = {};
    device13features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    device13features.dynamicRendering = true;
    device13features.synchronization2 = true;

    device12features = {};
    device12features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    device12features.bufferDeviceAddress = true;
    device12features.descriptorIndexing = true;

    sync2features = {};
    sync2features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    sync2features.synchronization2 = true;

    // thanks https://www.reddit.com/r/vulkan/comments/199e4bl/bug_in_vkguide_with_macosm1_when_selecting
    // turns out i cant use 1.3 after all :(
    deviceselector.set_minimum_version(1, 2);
    //deviceselector.set_required_features_13(device13features);
    deviceselector.set_required_features_12(device12features);
    deviceselector.add_required_extension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
    deviceselector.add_required_extension(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME);
    deviceselector.add_required_extension(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
    deviceselector.add_required_extension(VK_KHR_MULTIVIEW_EXTENSION_NAME);
    deviceselector.add_required_extension(VK_KHR_MAINTENANCE_2_EXTENSION_NAME);
    deviceselector.add_required_extension(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
    deviceselector.add_required_extension(VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME);
    deviceselector.add_required_extension_features(sync2features);
    deviceselector.set_surface(surface);

    vkb::Result<vkb::PhysicalDevice> physicalres = deviceselector.select();
    if(!physicalres)
    {
        printf("vulkan renderer couldn't find suitable physical device!\n");
        exit(1);
    }

    vkbphysicaldevice = physicalres.value();
    physicaldevice = vkbphysicaldevice.physical_device;

    printf("vulkan physical device selected.\n");
    
    vkb::DeviceBuilder devicebuilder(vkbphysicaldevice);
    vkb::Result<vkb::Device> deviceres = devicebuilder.build();
    if(!deviceres)
    {
        printf("error %d when trying to create vulkan device!\n", deviceres.vk_result());
        exit(1);
    }

	vkbdevice = deviceres.value();
    device = vkbdevice.device;
    
    pipelinebarrier2proc = (PFN_vkCmdPipelineBarrier2KHR) vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier2KHR");
    if(!pipelinebarrier2proc)
    {
        printf("error retrieving vkCmdPipelineBarrier2KHR procedure!\n");
        exit(1);
    }

    queuesubmit2proc = (PFN_vkQueueSubmit2KHR) vkGetDeviceProcAddr(device, "vkQueueSubmit2KHR");
    if(!queuesubmit2proc)
    {
        printf("error retrieving vkQueueSubmit2KHR procedure!\n");
        exit(1);
    }

    blitimage2proc = (PFN_vkCmdBlitImage2KHR) vkGetDeviceProcAddr(device, "vkCmdBlitImage2KHR");
    if(!blitimage2proc)
    {
        printf("error retrieving vkCmdBlitImage2KHR procedure!\n");
        exit(1);
    }

    printf("vulkan device created.\n");
}

void renderer::Renderer::Impl::VkInitInst(void)
{
#ifdef DEBUG
    const bool uselayers = true;
#else
    const bool uselayers = false;
#endif

    vkb::InstanceBuilder instbuilder;

    instbuilder.set_app_name("aegis");
    instbuilder.request_validation_layers(uselayers);
    instbuilder.use_default_debug_messenger();
    instbuilder.require_api_version(1, 2, 0);
    instbuilder.enable_extension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    // theyre making me declare a variable outside of the top of scope!
    // noooooo!!!!!
    vkb::Result<vkb::Instance> instres = instbuilder.build();
    if(!instres)
    {
        printf("error %d when trying to create vulkan instance!\n", instres.vk_result());
        exit(1);
    }

    vkbinst = instres.value();
    inst = vkbinst.instance;
    dbgmessenger = vkbinst.debug_messenger;

    printf("vulkan instance created.\n");
}

void renderer::Renderer::Impl::VkInit(void)
{
    int i;

    VkInitInst();
    VkInitDevice();
    VkInitVma();
    VkInitSwapchain();
    VkInitQues();
    for(i=0; i<max_fif; i++)
        renderer->frames[i].Init(renderer);
}

void renderer::Renderer::Impl::SDLInit(void)
{
    SDL_Init(SDL_INIT_VIDEO);

    win = SDL_CreateWindow(renderer->windowname.c_str(), 1280, 720, SDL_WINDOW_VULKAN);
}

void renderer::Renderer::Impl::Initialize(void)
{
    printf("initializing vulkan.\n");

    SDLInit();
    VkInit();
}

void renderer::Renderer::Impl::Shutdown(void)
{
    VkShutdown();
    SDLShutdown();
}

renderer::Frame* renderer::Renderer::CurFrame(void)
{
    return &frames[curframe % max_fif];
}

uint32_t renderer::Renderer::SwapchainImage(uint64_t timeoutns, Semaphore* sem, Fence* fence)
{
    VkResult res;
    uint32_t imageidx;
    VkSemaphore vksem;
    VkFence vkfence;

    vksem = NULL;
    vkfence = NULL;
    if(sem)
        vksem = sem->impl->vksem;
    if(fence)
        vkfence = fence->impl->vkfence;

    res = vkAcquireNextImageKHR(impl->device, impl->swapchain, timeoutns, vksem, vkfence, &imageidx);
    if(res != VK_SUCCESS)
    {
        printf("error %d with vkAcquireNextImageKHR!\n", res);
        exit(1);
    }

    return imageidx;
}

void renderer::Renderer::Present(uint32_t swapchainimg)
{
    VkResult res;
    VkPresentInfoKHR presentinfo;

    presentinfo = {};
    presentinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentinfo.pSwapchains = &impl->swapchain;
	presentinfo.swapchainCount = 1;
	presentinfo.pWaitSemaphores = &CurFrame()->rendersem.impl->vksem;
	presentinfo.waitSemaphoreCount = 1;
	presentinfo.pImageIndices = &swapchainimg;

	res = vkQueuePresentKHR(gfxque.impl->vkque, &presentinfo);

	curframe++;
}

void renderer::Renderer::Initialize(void)
{
    impl = std::make_unique<Impl>();
    impl->renderer = this;
    impl->Initialize();
}

void renderer::Renderer::Shutdown(void)
{
    if(!impl)
        return;
    impl->Shutdown();
}

renderer::Renderer::Renderer(void)
{

}

renderer::Renderer::~Renderer(void)
{

}