#pragma once

#include <memory>

#include <engine/cl_PlayerInput.h>

namespace engine::cl
{
    class Client
    {
    private:
        void Init(void);
    public:
        // games: set this to your own input class if you want to.
        std::unique_ptr<PlayerInput> pinput;

        void Setup(void);
        int Run(void);
    };
}