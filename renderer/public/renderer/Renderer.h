#pragma once

#include <memory>
#include <string>

namespace renderer
{
    class Frame;
    class Renderer;

    class Fence
    {
    private:
        renderer::Renderer *renderer = NULL;
    public:
        struct Impl;
        std::unique_ptr<Impl> impl;

        void Reset(void);
        // will block until the fence is signaled
        void Wait(uint64_t timeoutns);

        // called once at creation
        void Init(bool startsignaled, Renderer* renderer);
        void Shutdown(void);
    };

    class Semaphore
    {
    private:
        renderer::Renderer *renderer = NULL;
    public:
        struct Impl;
        std::unique_ptr<Impl> impl;

        // called once at creation
        void Init(Renderer* renderer);
        void Shutdown(void);
    };

    class CmdBuf
    {
    private:
        Renderer *renderer = NULL;
    public:
        struct Impl;
        std::unique_ptr<Impl> impl;

        // called once at creation
        void Init(Frame* frame, Renderer* renderer);
        void Shutdown(void);
    };

    class Frame
    {
    private:
        Renderer *renderer = NULL;
    public:
        CmdBuf maincmdbuf;
        
        Semaphore swapchainsem, rendersem;
        Fence renderfence;

        struct Impl;
        std::unique_ptr<Impl> impl;
    public:
        // called once at creation
        void Init(Renderer* renderer);
        void Shutdown(void);
    };

    class Renderer
    {
    public:
        Renderer(void);
        ~Renderer(void);
    public:
        static constexpr int max_fif = 2;

        std::string windowname = "aegis";

        Frame frames[max_fif];
        uint32_t curframe = 0;

        struct Impl;
        std::unique_ptr<Impl> impl;
    public:
        void Initialize(void);
        void Shutdown(void);
    };
}