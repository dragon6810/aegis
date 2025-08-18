#pragma once

#include <memory>
#include <string>

namespace renderer
{
    class Renderer
    {
    public:
        Renderer(void);
        ~Renderer(void);
    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    public:
        std::string windowname = "aegis";
    public:
        void Initialize(void);
        void Shutdown(void);
    };
}