#include "OpenGL.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SFML/Audio.hpp>

#include <iostream>

void OpenGL::GLInit()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW. Tough luck." << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
}

void OpenGL::PostWindowInit()
{
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW. Try using DirectX mode instead (if I stuck to my word and made one)." << std::endl;
        exit(EXIT_FAILURE);
    }
}