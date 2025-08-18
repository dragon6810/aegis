#include <renderer/Renderer.h>

#include <stdio.h>

#include <SDL3/SDL.h>
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

    void VkInit(void);
    void SDLInit(void);

    void Initialize(void);
    void Shutdown(void);
};

void renderer::Renderer::Impl::VkInit(void)
{

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