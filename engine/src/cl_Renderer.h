#pragma once

#include <renderer/Renderer.h>

#include <engine/cl_Client.h>

namespace engine::cl
{
    class Renderer
    {
    public:
        Renderer(Client& cl);
        ~Renderer(void);
    private:
        Client& cl;
        renderer::Renderer renderer;

        // cur frame data
        uint32_t iswapchain;
    public:
        void Render(void);

        void Init(void);
        void Shutdown(void);
    };
};