#include <stdio.h>

#include "Gui.h"

int main(int argc, char** argv)
{
    GLFWwindow *win;
    int w, h;

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    win = glfwCreateWindow(1280, 960, "World Painter 3d", NULL, NULL);
    glfwMakeContextCurrent(win);

    glewInit();
    if (!GLEW_EXT_framebuffer_object)
    {
        printf("couldnt enable fbo extension\n");
        return 1;
    }

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

    Gui::GetGui().Setup(win);

    while(!glfwWindowShouldClose(win))
    {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Gui::GetGui().Draw();

        glfwGetFramebufferSize(win, &w, &h);
        glViewport(0, 0, w, h);

        Gui::GetGui().FinishFrame();

        glfwSwapBuffers(win);
    }

    glfwTerminate();
}