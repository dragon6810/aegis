file(GLOB RENDER_SRC CONFIGURE_DEPENDS "src/vulkan/*.h" "src/vulkan/*.cpp")

add_library(renderer ${RENDER_SRC})

target_link_libraries(renderer
    PRIVATE
        SDL3::SDL3
        Vulkan::Headers
        Vulkan::Loader
        GPUOpen::VulkanMemoryAllocator
        vk-bootstrap::vk-bootstrap
        utilslib
        mathlib
    PUBLIC
        eigen
)

target_include_directories(renderer PUBLIC public/)