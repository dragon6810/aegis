#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include "mstudioload.h"
#include "rendermodel.h"
#include "loadtexture.h"
#include "binaryloader.h"
#include "AssetManager.h"
#include "BSPMap.h"
#include "Wad.h"

BSPMap map;

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

    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);  // Enable depth test
    glDepthFunc(GL_LEQUAL);   // Specify the depth function

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, 4.0 / 3.0, 1.0, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float d = 530;
    vec3_t camp = { d, d, d };
    gluLookAt(camp.x, camp.y, camp.z,
        0.0, 0.0, 0.0,
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

    Wad wad;
    wad.Load("valve/halflife.wad");
    
    map.Load("valve/maps/c4a1c.bsp");
    map.SetCameraPosition({ camp.x, camp.y, camp.z });

    long long lastFrame = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    while (!glfwWindowShouldClose(window))
    {
        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        long long delta = now - lastFrame;
        float fps = 1.0 / ((float)delta / 1000.0);

        //printf("%d FPS.\n", (int)fps);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear depth buffer too

        map.Draw();

        barney.render();
        hgrunt.render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        lastFrame = now;
    }
    
    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();

    AssetManager::getInst().cleanup();

    return 0;
}