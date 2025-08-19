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
	std::vector<VkImage> swapchainimgs;
	std::vector<VkImageView> swapchainviews;
	VkExtent2D swapchainextent;

    VkQueue gfxque;
	uint32_t gfxquefam;

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

void renderer::CmdBuf::Init(Frame* frame, Renderer* renderer)
{
    this->renderer = renderer;
    impl = std::make_unique<Impl>();
    impl->buf = this;
}

void renderer::CmdBuf::Shutdown(void)
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
	cmdpoolinfo.queueFamilyIndex = frame->renderer->impl->gfxquefam;

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
    vkb::Result<VkQueue> queres = vkbdevice.get_queue(vkb::QueueType::graphics).value();
    vkb::Result<uint32_t> famres = vkbdevice.get_queue_index(vkb::QueueType::graphics).value();
    if(!queres)
    {
        printf("failed to retrieve vulkan graphics queue!\n");
        exit(1);
    }
    if(!famres)
    {
        printf("failed to retrieve vulkan graphics queue family!\n");
        exit(1);
    }

	gfxque = queres.value();
    gfxquefam = famres.value();
}

void renderer::Renderer::Impl::VkInitSwapchain(void)
{
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
    swapchainimgs = vkbswapchain.get_images().value();
    swapchainviews = vkbswapchain.get_image_views().value();

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