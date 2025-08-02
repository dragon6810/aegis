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

    bool showconfigwindow = false;
    Cfglib::CfgFile workingcfg;
    bool smarteedit = true;
    int selectedpair = -1;
    std::unordered_map<std::string, std::vector<char>> valedit;
    std::unordered_map<std::string, std::vector<char>> keyedit;

    uint64_t lastframe = 0;

    void ApplyStyle(void);
    void DrawEntityPairs(void);
    void DrawPairHelper(void);
public:
    void Setup(GLFWwindow* win);
    void Draw();
    void FinishFrame();
};