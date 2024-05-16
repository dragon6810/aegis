#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "mstudioload.h"
#include "rendermodel.h"
#include "loadtexture.h"

const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 tex;
uniform mat4 projection;
uniform mat4 view;

out vec2 texcoord;

void main() 
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    texcoord = tex;
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

in vec2 texcoord;

uniform sampler2D albedosampler;

void main()
{
    FragColor = texture(albedosampler, texcoord);
}
)glsl";

int main() 
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Triangle", NULL, NULL);
    if (window == NULL) 
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    // Vertex Shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Fragment Shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Shader Program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    mstudioload barney = mstudioload();
    barney.load("models/barney.mdl");
    
    rendermodel modelrenderer = rendermodel(shaderProgram);

    GLuint* textures = (GLuint*) malloc(barney.header.numtextures);
    for (int t = 0; t < barney.header.numtextures; t++)
    {
        char* texdata = nullptr;
        int width = 0, height = 0;
        loadmstudiotexture(barney.data, t, TEXTYPE_MSTUDIO, (int**) &(texdata), &width, &height);
        glGenTextures(1, &textures[t]);
        glBindTexture(GL_TEXTURE_2D, textures[t]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata);
        free(texdata);
    }

    // Render loop
    while (!glfwWindowShouldClose(window)) 
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float pos[] = { 0.0F, 0.0F, 0.0F };

        modelrenderer.render(barney, pos, textures);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);

    for (int t = 0; t < barney.header.numtextures; t++)
        glDeleteTextures(1, &textures[t]);

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}