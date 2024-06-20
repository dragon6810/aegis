#include "Window.h"

Window::Window(std::string name, int width, int height, bool fullscreen)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if(fullscreen)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        width = mode->width;
        height = mode->height;
    }

    if(fullscreen)
        window = glfwCreateWindow(width, height, name.c_str(), monitor, nullptr);
    else
        window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (!window) 
    {
        printf("Failed to create GLFW window. Probably means I fucked up somewhere, but I'll blame it on your OS instead.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
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