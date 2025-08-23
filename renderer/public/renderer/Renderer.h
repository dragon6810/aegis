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
            LAYOUT_TRANSFER_SRC_OPTIMAL = 6,
            LAYOUT_TRANSFER_DST_OPTIMAL = 7,
            LAYOUT_DEPTH_ATTACHMENT_OPTIMAL = 1000241000,
            LAYOUT_PRESENT_SRC              = 1000001002,
        } layout_e;

        typedef enum
        {
            USAGE_TRANSFER_SRC             = 0x00000001,
            USAGE_TRANSFER_DST             = 0x00000002,
            USAGE_SAMPLED                  = 0x00000004,
            USAGE_STORAGE                  = 0x00000008,
            USAGE_COLOR_ATTACHMENT         = 0x00000010,
            USAGE_DEPTH_STENCIL_ATTACHMENT = 0x00000020,
            USAGE_TRANSIENT_ATTACHMENT     = 0x00000040,
            USAGE_INPUT_ATTACHMENT         = 0x00000080,
        } usage_e;

        typedef enum
        {
            FORMAT_UNDEFINED                  = 0,
            FORMAT_R4G4_UNORM_PACK8           = 1,
            FORMAT_R4G4B4A4_UNORM_PACK16      = 2,
            FORMAT_B4G4R4A4_UNORM_PACK16      = 3,
            FORMAT_R5G6B5_UNORM_PACK16        = 4,
            FORMAT_B5G6R5_UNORM_PACK16        = 5,
            FORMAT_R5G5B5A1_UNORM_PACK16      = 6,
            FORMAT_B5G5R5A1_UNORM_PACK16      = 7,
            FORMAT_A1R5G5B5_UNORM_PACK16      = 8,
            FORMAT_R8_UNORM                   = 9,
            FORMAT_R8_SNORM                   = 10,
            FORMAT_R8_USCALED                 = 11,
            FORMAT_R8_SSCALED                 = 12,
            FORMAT_R8_UINT                    = 13,
            FORMAT_R8_SINT                    = 14,
            FORMAT_R8_SRGB                    = 15,
            FORMAT_R8G8_UNORM                 = 16,
            FORMAT_R8G8_SNORM                 = 17,
            FORMAT_R8G8_USCALED               = 18,
            FORMAT_R8G8_SSCALED               = 19,
            FORMAT_R8G8_UINT                  = 20,
            FORMAT_R8G8_SINT                  = 21,
            FORMAT_R8G8_SRGB                  = 22,
            FORMAT_R8G8B8_UNORM               = 23,
            FORMAT_R8G8B8_SNORM               = 24,
            FORMAT_R8G8B8_USCALED             = 25,
            FORMAT_R8G8B8_SSCALED             = 26,
            FORMAT_R8G8B8_UINT                = 27,
            FORMAT_R8G8B8_SINT                = 28,
            FORMAT_R8G8B8_SRGB                = 29,
            FORMAT_B8G8R8_UNORM               = 30,
            FORMAT_B8G8R8_SNORM               = 31,
            FORMAT_B8G8R8_USCALED             = 32,
            FORMAT_B8G8R8_SSCALED             = 33,
            FORMAT_B8G8R8_UINT                = 34,
            FORMAT_B8G8R8_SINT                = 35,
            FORMAT_B8G8R8_SRGB                = 36,
            FORMAT_R8G8B8A8_UNORM             = 37,
            FORMAT_R8G8B8A8_SNORM             = 38,
            FORMAT_R8G8B8A8_USCALED           = 39,
            FORMAT_R8G8B8A8_SSCALED           = 40,
            FORMAT_R8G8B8A8_UINT              = 41,
            FORMAT_R8G8B8A8_SINT              = 42,
            FORMAT_R8G8B8A8_SRGB              = 43,
            FORMAT_B8G8R8A8_UNORM             = 44,
            FORMAT_B8G8R8A8_SNORM             = 45,
            FORMAT_B8G8R8A8_USCALED           = 46,
            FORMAT_B8G8R8A8_SSCALED           = 47,
            FORMAT_B8G8R8A8_UINT              = 48,
            FORMAT_B8G8R8A8_SINT              = 49,
            FORMAT_B8G8R8A8_SRGB              = 50,
            FORMAT_A8B8G8R8_UNORM_PACK32      = 51,
            FORMAT_A8B8G8R8_SNORM_PACK32      = 52,
            FORMAT_A8B8G8R8_USCALED_PACK32    = 53,
            FORMAT_A8B8G8R8_SSCALED_PACK32    = 54,
            FORMAT_A8B8G8R8_UINT_PACK32       = 55,
            FORMAT_A8B8G8R8_SINT_PACK32       = 56,
            FORMAT_A8B8G8R8_SRGB_PACK32       = 57,
            FORMAT_A2R10G10B10_UNORM_PACK32   = 58,
            FORMAT_A2R10G10B10_SNORM_PACK32   = 59,
            FORMAT_A2R10G10B10_USCALED_PACK32 = 60,
            FORMAT_A2R10G10B10_SSCALED_PACK32 = 61,
            FORMAT_A2R10G10B10_UINT_PACK32    = 62,
            FORMAT_A2R10G10B10_SINT_PACK32    = 63,
            FORMAT_A2B10G10R10_UNORM_PACK32   = 64,
            FORMAT_A2B10G10R10_SNORM_PACK32   = 65,
            FORMAT_A2B10G10R10_USCALED_PACK32 = 66,
            FORMAT_A2B10G10R10_SSCALED_PACK32 = 67,
            FORMAT_A2B10G10R10_UINT_PACK32    = 68,
            FORMAT_A2B10G10R10_SINT_PACK32    = 69,
            FORMAT_R16_UNORM                  = 70,
            FORMAT_R16_SNORM                  = 71,
            FORMAT_R16_USCALED                = 72,
            FORMAT_R16_SSCALED                = 73,
            FORMAT_R16_UINT                   = 74,
            FORMAT_R16_SINT                   = 75,
            FORMAT_R16_SFLOAT                 = 76,
            FORMAT_R16G16_UNORM               = 77,
            FORMAT_R16G16_SNORM               = 78,
            FORMAT_R16G16_USCALED             = 79,
            FORMAT_R16G16_SSCALED             = 80,
            FORMAT_R16G16_UINT                = 81,
            FORMAT_R16G16_SINT                = 82,
            FORMAT_R16G16_SFLOAT              = 83,
            FORMAT_R16G16B16_UNORM            = 84,
            FORMAT_R16G16B16_SNORM            = 85,
            FORMAT_R16G16B16_USCALED          = 86,
            FORMAT_R16G16B16_SSCALED          = 87,
            FORMAT_R16G16B16_UINT             = 88,
            FORMAT_R16G16B16_SINT             = 89,
            FORMAT_R16G16B16_SFLOAT           = 90,
            FORMAT_R16G16B16A16_UNORM         = 91,
            FORMAT_R16G16B16A16_SNORM         = 92,
            FORMAT_R16G16B16A16_USCALED       = 93,
            FORMAT_R16G16B16A16_SSCALED       = 94,
            FORMAT_R16G16B16A16_UINT          = 95,
            FORMAT_R16G16B16A16_SINT          = 96,
            FORMAT_R16G16B16A16_SFLOAT        = 97,
            FORMAT_R32_UINT                   = 98,
            FORMAT_R32_SINT                   = 99,
            FORMAT_R32_SFLOAT                 = 100,
            FORMAT_R32G32_UINT                = 101,
            FORMAT_R32G32_SINT                = 102,
            FORMAT_R32G32_SFLOAT              = 103,
            FORMAT_R32G32B32_UINT             = 104,
            FORMAT_R32G32B32_SINT             = 105,
            FORMAT_R32G32B32_SFLOAT           = 106,
            FORMAT_R32G32B32A32_UINT          = 107,
            FORMAT_R32G32B32A32_SINT          = 108,
            FORMAT_R32G32B32A32_SFLOAT        = 109,
            FORMAT_R64_UINT                   = 110,
            FORMAT_R64_SINT                   = 111,
            FORMAT_R64_SFLOAT                 = 112,
            FORMAT_R64G64_UINT                = 113,
            FORMAT_R64G64_SINT                = 114,
            FORMAT_R64G64_SFLOAT              = 115,
            FORMAT_R64G64B64_UINT             = 116,
            FORMAT_R64G64B64_SINT             = 117,
            FORMAT_R64G64B64_SFLOAT           = 118,
            FORMAT_R64G64B64A64_UINT          = 119,
            FORMAT_R64G64B64A64_SINT          = 120,
            FORMAT_R64G64B64A64_SFLOAT        = 121,
        } format_e;

        typedef enum
        {
            ASPECT_COLOR_BIT   = 0x00000001,
            ASPECT_DEPTH_BIT   = 0x00000002,
            ASPECT_STENCIL_BIT = 0x00000004,
        } aspect_e;
    private:
        Renderer *renderer = NULL;
    public:
        struct Impl;
        std::unique_ptr<Impl> impl;
        Eigen::Vector2i size;
        Image::layout_e layout = LAYOUT_UNDEFINED;
        uint32_t usage;
        Image::format_e format;
    public:
        // cmdbuf must be in a recording state
        void TransitionLayout(CmdBuf* cmdbuf, layout_e dstlayout);
        void BlitToImage(CmdBuf* cmdbuf, Image* dst);
        void ClearColor(CmdBuf* cmdbuf, Eigen::Vector3f col);

        void Create(Eigen::Vector2i size, uint32_t usageflags, uint32_t aspectflags, Image::format_e fmt);

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

        Image drawimg;
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