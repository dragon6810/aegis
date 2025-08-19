#pragma once

#include <memory>
#include <string>

#include <Eigen/Dense>

namespace renderer
{
    class CmdBuf;
    class Frame;
    class Renderer;

    typedef enum
    {
        STAGE_TRANSFER                = 0x00001000ULL,
        STAGE_ALL_GRAPHICS            = 0x00008000ULL,
        STAGE_COLOR_ATTACHMENT_OUTPUT = 0x00000400ULL,
    } stageflags_e;

    class Image
    {
    public:
        typedef enum
        {
            LAYOUT_UNDEFINED                = 0,
            LAYOUT_GENERAL                  = 1,
            LAYOUT_DEPTH_ATTACHMENT_OPTIMAL = 1000241000,
            LAYOUT_PRESENT_SRC              = 1000001002,
        } layout_e;
    private:
        Renderer *renderer = NULL;
    public:
        struct Impl;
        std::unique_ptr<Impl> impl;
    public:
        // cmdbuf must be in a recording state
        void TransitionLayout(CmdBuf* cmdbuf, layout_e srclayout, layout_e dstlayout);

        // doesnt' create the image itself
        void Init(Renderer* renderer);
        void Shutdown(void);
    };

    class FreeImage
    {
    private:
        Renderer *renderer = NULL;
    public:
        Image img;
        Image::layout_e layout;

        struct Impl;
        std::unique_ptr<Impl> impl;
    public:
        // cmdbuf must be in a recording state
        void TransitionLayout(CmdBuf* cmdbuf, Image::layout_e dstlayout);

        // doesnt' create the image itself
        void Init(Renderer* renderer);
        void Shutdown(void);
    };

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
    public:
        typedef enum
        {
            USAGE_ONE_TIME_SUBMIT      = 0x00000001,
            USAGE_RENDER_PASS_CONTINUE = 0x00000002,
            USAGE_SIMULTANEOUS_USE     = 0x00000004,
        } usageflags_e;
    private:
        Renderer *renderer = NULL;
    public:
        struct Impl;
        std::unique_ptr<Impl> impl;

        // commands
        void CmdClearColorImage(Image* img, Image::layout_e imglayout, Eigen::Vector3f col);

        void Reset(bool releaseresources);
        void Begin(uint32_t usageflags);
        void End(void);

        // called once at creation
        void Init(Frame* frame, Renderer* renderer);
        void Shutdown(void);
    };

    class Queue
    {
    public:
        typedef enum
        {
            TYPE_GFX,
            TYPE_COMP,
            TYPE_TRANSFER,
        } type_e;
    private:
        Renderer *renderer = NULL;
    public:
        struct Impl;
        std::unique_ptr<Impl> impl;
    public:
        // buf must be valid. waitsem, signalsem, and signalfence can all be NULL.
        void SubmitCmdBuf(CmdBuf* buf, Semaphore* waitsem, Semaphore* signalsem, Fence* signalfence, stageflags_e waitstage, stageflags_e signalstage);

        void Init(type_e type, Renderer* renderer);
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

        std::vector<Image> swapchainimgs;
        Queue gfxque;

        Frame frames[max_fif];
        uint32_t curframe = 0;

        struct Impl;
        std::unique_ptr<Impl> impl;
    public:
        Frame* CurFrame(void);
        // get the next image from the swapchain to render to
        // sem and fence can both be NULL if you don't want to signal any
        uint32_t SwapchainImage(uint64_t timeoutns, Semaphore* sem, Fence* fence);

        void Present(uint32_t swapchainimg);

        void Initialize(void);
        void Shutdown(void);
    };
}