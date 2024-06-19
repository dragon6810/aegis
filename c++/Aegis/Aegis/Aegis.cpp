#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include "mstudioload.h"
#include "SModel.h"
#include "loadtexture.h"
#include "binaryloader.h"
#include "AssetManager.h"
#include "BSPMap.h"
#include "Wad.h"
#include "mathutils.h"

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

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Aegis", nullptr, nullptr);
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
    gluPerspective(65, 16.0 / 9.0, 1.0, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float d = 300;
    vec3_t camp = { d, d, d };
    vec3_t camf = { 0, 0, 75 };
    gluLookAt(camp.x, camp.y, camp.z,
        camf.x, camf.y, camf.z,
        0.0, 0.0, 1.0);

    glViewport(0, 0, 1920, 1080);

    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);

    Wad wad;
    wad.Load("valve/halflife.wad");
    
    map.Load("valve/maps/test.bsp");
    map.SetCameraPosition({ camp.x, camp.y, camp.z });
    map.cameraforward = NormalizeVector3({ camf.x - camp.x, camf.y - camp.y, camf .z - camp.z });
    map.cameraup = { 0, 0, 1 };
    map.sky.campos = camp;

    long long lastFrame = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    while (!glfwWindowShouldClose(window))
    {
        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        long long delta = now - lastFrame;
        float fps = 1.0 / ((float)delta / 1000.0);

        //printf("%d FPS.\n", (int)fps);

        glClear(GL_DEPTH_BUFFER_BIT);

        //map.sky.Render();

        map.Think(1.0 / fps);
        map.Draw();

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