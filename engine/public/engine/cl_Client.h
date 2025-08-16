#pragma once

#include <memory>

#include <SDL3/SDL.h>

#include <engine/cl_PlayerInput.h>

namespace engine::cl
{
    class Client
    {
    private:
        SDL_Window *win = NULL;
    private:
        void MakeWindow(void);
        void PollWindow(void);
        void DestroyWindow(void);
        void Init(void);
    public:
        bool lastframe = false;

        // games: set this to your own input class if you want to.
        std::unique_ptr<PlayerInput> pinput;

        void Setup(void);
        int Run(void);
    };
}