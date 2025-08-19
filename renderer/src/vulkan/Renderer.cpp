#include <renderer/Renderer.h>

#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>

#include <utilslib.h>

// i have a feeling this file will get very, very large.

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
	VkSurfaceKHR surface;
    SDL_Window *win = NULL;

    VkSwapchainKHR swapchain;
	VkFormat swapchainformat;
	std::vector<VkImageView> swapchainviews;
	VkExtent2D swapchainextent;

    PFN_vkCmdPipelineBarrier2KHR pipelinebarrier2proc;
    PFN_vkQueueSubmit2KHR queuesubmit2proc;

    void VkShutdownInst(void);
    void VkShutdownDevice(void);
    void VkShutdownSwapchain(void);
    void VkShutdown(void);
    void SDLShutdown(void);

    void VkInitQues(void);
    void VkInitSwapchain(void);
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

struct renderer::Image::Impl
{
    VkImage vkimg;
};

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

void renderer::CmdBuf::CmdClearColorImage(Image* img, Image::layout_e imglayout, Eigen::Vector3f col)
{
    int i;

    VkImageSubresourceRange subresource;

    VkClearColorValue vkcol;

    UTILS_ASSERT(img);

    for(i=0; i<3; i++)
        vkcol.float32[i] = col[i];
    vkcol.float32[i] = 1; // always opaque for now

    subresource = {};
    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource.baseMipLevel = 0;
    subresource.levelCount = VK_REMAINING_MIP_LEVELS;
    subresource.baseArrayLayer = 0;
    subresource.layerCount = VK_REMAINING_ARRAY_LAYERS;

    vkCmdClearColorImage(impl->cmdbuf, img->impl->vkimg, (VkImageLayout) imglayout, &vkcol, 1, &subresource);
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

void renderer::Queue::SubmitCmdBuf(CmdBuf* buf, Semaphore* waitsem, Semaphore* signalsem, Fence* signalfence, stageflags_e waitstage, stageflags_e signalstage)
{
    VkSubmitInfo2KHR submitinfo;
    VkSemaphoreSubmitInfoKHR waitseminfo, signalseminfo;
    VkCommandBufferSubmitInfoKHR cmdbufinfo;

    UTILS_ASSERT(renderer);
    UTILS_ASSERT(impl.get());

    UTILS_ASSERT(buf);

    submitinfo = {};
    submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR;

    if(waitsem)
    {
        waitseminfo = {};
        waitseminfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
        waitseminfo.semaphore = waitsem->impl->vksem;
        waitseminfo.stageMask = waitstage;
        waitseminfo.deviceIndex = 0;
        waitseminfo.value = 1;

        submitinfo.waitSemaphoreInfoCount = 1;
        submitinfo.pWaitSemaphoreInfos = &waitseminfo;
    }

    if(signalsem)
    {
        signalseminfo = {};
        signalseminfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
        signalseminfo.semaphore = signalsem->impl->vksem;
        signalseminfo.stageMask = signalstage;
        signalseminfo.deviceIndex = 0;
        signalseminfo.value = 1;

        submitinfo.signalSemaphoreInfoCount = 1;
        submitinfo.pSignalSemaphoreInfos = &signalseminfo;
    }

    cmdbufinfo = {};
    cmdbufinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
    cmdbufinfo.commandBuffer = buf->impl->cmdbuf;
	cmdbufinfo.deviceMask = 0;

    submitinfo.commandBufferInfoCount = 1;
    submitinfo.pCommandBufferInfos = &cmdbufinfo;

    if(signalfence)
        renderer->impl->queuesubmit2proc(impl->vkque, 1, &submitinfo, signalfence->impl->vkfence);
    else
        renderer->impl->queuesubmit2proc(impl->vkque, 1, &submitinfo, NULL);
}

void renderer::Queue::Init(type_e type, Renderer* renderer)
{
    this->renderer = renderer;
    this->impl = std::make_unique<Impl>();

    vkb::Result<VkQueue> queres = vkb::Result<VkQueue>(vkb::Error());
    vkb::Result<uint32_t> famres = vkb::Result<uint32_t>(vkb::Error());
    switch(type)
    {
    case TYPE_GFX:
        queres = renderer->impl->vkbdevice.get_queue(vkb::QueueType::graphics).value();
        famres = renderer->impl->vkbdevice.get_queue_index(vkb::QueueType::graphics).value();
        break;
    default:
        UTILS_ASSERT(0);
        break;
    }

    if(!queres)
    {
        printf("error %d getting vkQueue!\n", queres.vk_result());
        exit(1);
    }

    if(!queres)
    {
        printf("error %d getting vkQueueFamilyIndex!\n", famres.vk_result());
        exit(1);
    }

	impl->vkque = queres.value();
    impl->quefam = famres.value();
}

void renderer::Queue::Shutdown(void)
{
    
}

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

void renderer::Renderer::Impl::VkShutdownSwapchain(void)
{
    int i;

    vkDestroySwapchainKHR(device, swapchain, NULL);
    for(i=0; i<swapchainviews.size(); i++)
        vkDestroyImageView(device, swapchainviews[i], NULL);
}

void renderer::Renderer::Impl::VkShutdown(void)
{
    int i;

    vkDeviceWaitIdle(device);

    // VkShutdownInst(); // validation layer test

    for(i=0; i<max_fif; i++)
        renderer->frames[i].Shutdown();

    VkShutdownSwapchain();
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
    swapchainextent.width = w;
    swapchainextent.height = h;

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
    swapchainviews = vkbswapchain.get_image_views().value();

    renderer->swapchainimgs.resize(vkbswapchain.get_images().value().size());
    for(i=0; i<renderer->swapchainimgs.size(); i++)
    {
        renderer->swapchainimgs[i].Init(renderer);
        renderer->swapchainimgs[i].impl->vkimg = vkbswapchain.get_images().value()[i];
    }

    printf("vulkan/SDL3 swapchain created.\n");
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