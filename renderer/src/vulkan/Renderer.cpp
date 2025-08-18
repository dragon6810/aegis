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

    void VkShutdownInst(void);
    void VkShutdownDevice(void);
    void VkShutdownSwapchain(void);
    void VkShutdown(void);
    void SDLShutdown(void);

    void VkInitSwapchain(void);
    void VkInitDevice(void);
    void VkInitInst(void);
    void VkInit(void);
    void SDLInit(void);

    void Initialize(void);
    void Shutdown(void);
};

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
    // VkShutdownInst(); // validation layer test
    VkShutdownSwapchain();
    VkShutdownDevice();
    VkShutdownInst();
}

void renderer::Renderer::Impl::SDLShutdown(void)
{
    SDL_DestroyWindow(win);
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
    VkInitInst();
    VkInitDevice();
    VkInitSwapchain();
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