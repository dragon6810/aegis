#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Gui.h"

int main(int argc, char** argv)
{
    GLFWwindow* win;

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    win = glfwCreateWindow(1280, 960, "World Painter 3d", NULL, NULL);
    glfwMakeContextCurrent(win);

    glewInit();
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

    Gui::GetGui().Setup(win);

    while(!glfwWindowShouldClose(win))
    {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Gui::GetGui().Draw();

        Gui::GetGui().FinishFrame();

        glfwSwapBuffers(win);
    }

    glfwTerminate();
}