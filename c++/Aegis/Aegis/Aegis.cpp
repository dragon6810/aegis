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
#include "Game.h"

#include <cassert>

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

    Game::GetGame().Main(); 

    return 0;
}