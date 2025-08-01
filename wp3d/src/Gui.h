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
private:
    Gui() {}

    Map map;
    Viewport viewports[Viewport::NTYPES];
    int currentviewport = -1;

    bool showconfigwindow = false;
    Cfglib::CfgFile workingcfg;
    bool smarteedit = true;
    std::unordered_map<std::string, std::vector<char>> valedit;
    std::unordered_map<std::string, std::vector<char>> keyedit;

    uint64_t lastframe = 0;

    void ApplyStyle(void);
    void DrawMenuBar(void);
    void DrawConfigMenu(void);
    void DrawViewports(float deltatime);
    void DrawToolBar(void);
    void DrawToolSettings(void);
    void DrawEntityPairs(void);
    void DrawRibbon(void);
    void ViewportInput(void);
public:
    void Setup(GLFWwindow* win);
    void Draw();
    void FinishFrame();
};