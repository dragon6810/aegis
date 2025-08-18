file(GLOB RENDER_SRC CONFIGURE_DEPENDS "src/vulkan/*.h" "src/vulkan/*.cpp")

add_library(renderer ${RENDER_SRC})

find_package(Vulkan REQUIRED)

target_link_libraries(renderer PRIVATE
    Vulkan::Vulkan
    utilslib
    mathlib
)

target_link_libraries(renderer PUBLIC
    eigen
)

target_include_directories(renderer PUBLIC public/)