#include <stdio.h>

#include "Gui.h"

#include <GLFW/glfw3.h>

void errorcallback(int error, const char* description) 
{
    printf("GLFW Error: %d: \"%s\".\n", error, description);
}

int main(int argc, char** argv)
{
    GLFWwindow *win;
    int w, h;
    Gui gui;

    glfwSetErrorCallback(errorcallback);
    printf("GLFW Version: %s\n", glfwGetVersionString());
    if(!glfwInit())
    {
        printf("couldn't initialize glfw.\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    win = glfwCreateWindow(1280, 960, "World Painter 3d", NULL, NULL);
    if(!win)
    {
        printf("couldn't create window.\n");
        return 1;
    }

    glfwMakeContextCurrent(win);

    if(glewInit() != GLEW_OK)
    {
        printf("couldn't initialize glew.\n");
        return 1;
    }
    if (!GLEW_EXT_framebuffer_object)
    {
        printf("couldnt enable fbo extension\n");
        return 1;
    }

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

    gui = Gui();
    gui.Setup(win);

    while(!glfwWindowShouldClose(win))
    {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gui.Draw();

        glfwGetFramebufferSize(win, &w, &h);
        glViewport(0, 0, w, h);

        gui.FinishFrame();

        glfwSwapBuffers(win);
    }

    glfwTerminate();
}
