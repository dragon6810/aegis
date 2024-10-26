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
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
add_executable(Aegis ${SOURCES})

# Link with the .lib files (adjust paths to match your setup)
target_link_libraries(Aegis
    ${CMAKE_SOURCE_DIR}/external/glew/lib/glew32.lib
    ${CMAKE_SOURCE_DIR}/external/glfw/lib/glfw3.lib
    ${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-system.lib
    ${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-window.lib
    ${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-graphics.lib
    ${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-audio.lib
)

# Platform-specific settings
#if (WIN32)
#    # Link OpenGL on Windows
#    target_link_libraries(Aegis opengl32)
#elseif(APPLE)
#    # Link OpenGL on macOS
#    target_link_libraries(Aegis "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
#endif()
