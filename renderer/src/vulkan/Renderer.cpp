#include <renderer/Renderer.h>

#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>

#include <utilslib.h>

struct renderer::Renderer::Impl
{
    renderer::Renderer *renderer = NULL;

    VkInstance inst;
	VkDebugUtilsMessengerEXT dbgmessenger;
	VkPhysicalDevice physicaldevice;
	VkDevice device;
	VkSurfaceKHR surface;
    SDL_Window *win = NULL;

    void VkInitDevice(void);
    void VkInitInst(void);
    void VkInit(void);
    void SDLInit(void);

    void Initialize(void);
    void Shutdown(void);
};

void renderer::Renderer::Impl::VkInitDevice(void)
{
    if(!SDL_Vulkan_CreateSurface(win, inst, NULL, &surface))
    {
        printf("error creating SDL vulkan surface:\n%s\n", SDL_GetError());
        exit(1);
    }
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

    inst = instres.value();
    dbgmessenger = instres.value().debug_messenger;

    printf("vulkan instance created.\n");
}

void renderer::Renderer::Impl::VkInit(void)
{
    VkInitInst();
    VkInitDevice();
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
    printf("cleaning up vulkan.\n");
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