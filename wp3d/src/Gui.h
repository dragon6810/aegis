#pragma once

#include <memory>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GuiElement.h"
#include "Map.h"
#include "Viewport.h"

#ifdef __MACOS
    #define GUI_CONTROL_STRING "Cmd"
    #define GUI_ALT_STRING "Opt"
#else
    #define GUI_CONTROL_STRING "Ctrl"
    #define GUI_ALT_STRING "Alt"
#endif

class Gui
{
private:
    Map map;
    std::vector<std::unique_ptr<GuiElement>> elements;

    void ApplyStyle(void);
public:
    void Setup(GLFWwindow* win);
    void Draw();
    void FinishFrame();
};