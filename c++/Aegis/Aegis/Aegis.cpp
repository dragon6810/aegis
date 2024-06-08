#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include "mstudioload.h"
#include "rendermodel.h"
#include "loadtexture.h"
#include "binaryloader.h"
#include "AssetManager.h"

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Debug", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    //glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);  // Enable depth test
    glDepthFunc(GL_LEQUAL);   // Specify the depth function

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, 4.0 / 3.0, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(53.0, 53.0, 88.0,
        0.0, 0.0, 35.0,
        0.0, 0.0, 1.0);

    glViewport(0, 0, 800, 600);

    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);

    SModel barney;
    barney.Load("valve/models/barney.mdl");
    barney.startseq(4);
    barney.SetPosition(-20.0, 0.0, 0.0);

    SModel hgrunt;
    hgrunt.Load("valve/models/hgrunt.mdl");
    hgrunt.startseq(0);
    hgrunt.SetPosition(20.0, 0.0, 0.0);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear depth buffer too

        glDisable(GL_CULL_FACE);

        barney.render();
        hgrunt.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();

    AssetManager::getInst().cleanup();

    return 0;
}