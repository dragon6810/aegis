#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Map.h"
#include "Viewport.h"

class Gui
{
public:
    static Gui& GetGui()
    {
        static Gui instance;
        return instance;
    }

    typedef enum
    {
        TOOL_BRUSH,
    } tool_e;
private:
    Gui() {}

    Map map;
    Viewport viewports[Viewport::NTYPES];
    int currentviewport = -1;
    tool_e currenttool;

    uint64_t lastframe = 0;

    void DrawViewports(float deltatime);
    void ViewportInput(void);
public:
    void Setup(GLFWwindow* win);
    void Draw();
    void FinishFrame();
};