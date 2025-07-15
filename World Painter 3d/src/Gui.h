#pragma once

#include <GLFW/glfw3.h>

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
public:
    void Setup(GLFWwindow* win);
    void Draw();
    void FinishFrame();
};