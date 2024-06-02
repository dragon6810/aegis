#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "mstudioload.h"
#include "rendermodel.h"
#include "loadtexture.h"
#include "binaryloader.h"

void renderCube()
{
    glBegin(GL_QUADS);

    // Front face
    glColor3f(1.0f, 0.0f, 0.0f);  // Red
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);

    // Back face
    glColor3f(0.0f, 1.0f, 0.0f);  // Green
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);

    // Left face
    glColor3f(0.0f, 0.0f, 1.0f);  // Blue
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);

    // Right face
    glColor3f(1.0f, 1.0f, 0.0f);  // Yellow
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);

    // Top face
    glColor3f(1.0f, 0.0f, 1.0f);  // Magenta
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);

    // Bottom face
    glColor3f(0.0f, 1.0f, 1.0f);  // Cyan
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);

    glEnd();
}

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

    // Initialize model
    mstudioload barney = mstudioload();
    barney.load("valve/models/barney.mdl");

    rendermodel modelrenderer = rendermodel(0);
    mstudioload texmodel = barney;

    if (barney.header.numtextures == 0)
    {
        char texturename[256];
        strcpy(texturename, barney.header.name);
        strcpy(&texturename[strlen(texturename) - 4], "T.mdl");
        texmodel.load(texturename);
    }

    mstudioheader_t* textureHeader = (mstudioheader_t*)texmodel.data;
    GLuint* textures = (GLuint*)malloc(textureHeader->numtextures * sizeof(GLuint));

    for (int t = 0; t < textureHeader->numtextures; t++)
    {
        char* texdata = nullptr;
        int width = 0, height = 0;
        loadmstudiotexture(texmodel.data, t, TEXTYPE_MSTUDIO, (int**)&(texdata), &width, &height);
        glGenTextures(1, &textures[t]);
        glBindTexture(GL_TEXTURE_2D, textures[t]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata);
        free(texdata);
    }

    //glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);  // Enable depth test
    glDepthFunc(GL_LEQUAL);   // Specify the depth function

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, 4.0 / 3.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(10.0, 10.0, 10.0,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0);

    glViewport(0, 0, 800, 600);

    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear depth buffer too

        glDisable(GL_CULL_FACE);

        float pos[] = { 0.0f, 0.0f, 0.0f };
        modelrenderer.render(barney, pos, textures, texmodel);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (int t = 0; t < texmodel.header.numtextures; t++)
        glDeleteTextures(1, &textures[t]);

    free(textures);

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}