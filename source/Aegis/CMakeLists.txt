# TO COMPILE ON MAC:
# brew install glew glfw sfml

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
add_executable(Aegis ${SOURCES} "src/main.cpp" "src/Game.cpp" "src/Game.h" "src/World.cpp" "src/World.h" "src/Window.cpp" "src/Window.h" "src/Renderer.cpp" "src/Renderer.h" "src/Command.cpp" "src/ResourceManager.cpp" "src/ResourceManager.h" "src/Input.cpp" "src/Input.h")

if (APPLE)
    find_package(GLEW REQUIRED)
    find_package(GLFW3 REQUIRED)
    find_package(SFML COMPONENTS system audio REQUIRED)
    
    target_link_libraries(Aegis PRIVATE
        GLEW::GLEW
        glfw
        sfml-system
        sfml-audio
        "-framework OpenGL"
    )
elseif (WIN32)
    target_link_libraries(Aegis PRIVATE
        ${GLEW_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${SFML_LIBRARIES}
        opengl32
    )
endif()