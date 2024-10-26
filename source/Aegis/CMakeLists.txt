cmake_minimum_required(VERSION 3.10)
project(Aegis)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories for all the libraries
include_directories(
    external/glew/include
    external/glfw/include
    external/glm
    external/SFML/include
)

# Add the executable
add_executable(Aegis ${SOURCES} "src/main.cpp" "src/Game.cpp" "src/Game.h" "src/World.cpp" "src/World.h" "src/Window.cpp" "src/Window.h" "src/Renderer.cpp" "src/Renderer.h")

# Link with the .lib files (adjust paths to match your setup)
target_link_libraries(Aegis
    ${CMAKE_SOURCE_DIR}/external/glew/lib/glew32.lib
    ${CMAKE_SOURCE_DIR}/external/glfw/lib/glfw3.lib
    ${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-system.lib
    ${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-audio.lib
)

# Platform-specific settings
if (WIN32)
    # Link OpenGL on Windows
    target_link_libraries(Aegis opengl32)
elseif(APPLE)
    # Link OpenGL on macOS
    target_link_libraries(Aegis "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()
