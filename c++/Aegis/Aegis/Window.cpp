#include "Window.h"

#include <iostream>

Window::Window(std::string name, int width, int height, bool fullscreen) 
{
    if (!glfwInit()) 
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (fullscreen) 
    {
        width = mode->width - 1;
        height = mode->height;

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
    }

    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (!window) 
    {
        std::cerr << "Failed to create GLFW window. Probably means I messed up somewhere, but I'll blame it on your OS instead." << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwMakeContextCurrent(window);
}

Window::~Window()
{
    if (window != nullptr)
        Kill();
}

void Window::Kill()
{
    glfwDestroyWindow(window);
    window = nullptr;
}

void Window::SelectForRendering()
{
    glfwMakeContextCurrent(window);
}

void Window::MakeFullscreenViewport(float aspect)
{
    int width;
    int height;
    GetWindowDimensions(&width, &height);
    float windowAspect = (float)width / (float)height;

    int viewwidth;
    int viewheight;

    int viewx;
    int viewy;

    if (windowAspect > aspect) 
    {
        // Window is wider than content aspect ratio
        viewheight = height;
        viewwidth = (int)(viewheight * aspect);
        viewx = (width - viewwidth) / 2;
        viewy = 0;
    }
    else 
    {
        // Window is taller than content aspect ratio
        viewwidth = width;
        viewheight = (int)(viewwidth / aspect);
        viewx = 0;
        viewy = (height - viewheight) / 2;
    }

    glViewport(viewx, viewy, viewwidth, viewheight);
}

void Window::SetKeyCallback(GLFWkeyfun func)
{
    glfwSetKeyCallback(window, (GLFWkeyfun) func);
}

void Window::SetCursorPosCallback(GLFWcursorposfun func)
{
    glfwSetCursorPosCallback(window, func);
}

void Window::GetWindowDimensions(int* x, int* y)
{
    glfwGetWindowSize(window, x, y);
}

bool Window::ShouldWindowClose()
{
    return glfwWindowShouldClose(window);
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(window);
}