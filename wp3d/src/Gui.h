#pragma once

#include <memory>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GuiElement.h"
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
    std::vector<std::unique_ptr<GuiElement>> elements;

    bool showconfigwindow = false;
    Cfglib::CfgFile workingcfg;
    bool smarteedit = true;
    int selectedpair = -1;
    std::unordered_map<std::string, std::vector<char>> valedit;
    std::unordered_map<std::string, std::vector<char>> keyedit;

    uint64_t lastframe = 0;

    void ApplyStyle(void);
    void DrawMenuBar(void);
    void DrawConfigMenu(void);
    void DrawToolBar(void);
    void DrawToolSettings(void);
    void DrawEntityPairs(void);
    void DrawPairHelper(void);
    void DrawRibbon(void);
    void ViewportInput(void);
public:
    void Setup(GLFWwindow* win);
    void Draw();
    void FinishFrame();
};