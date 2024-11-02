#pragma once

#include "ResourceManager.h"
#include "RasterFont.h"

class Console
{
public:
    ~Console();

    std::string output;

    static void Print(const char* format, ...);

    void Load();
    void Render();

    void Toggle();
    void KeyInput(char c);

    bool IsDown();
private:
    bool isdown;

    ResourceManager::texture_t* consback;
    RasterFont font;
    float curoffs = 240;
    const int lowoffs = 160;
    const int visheight = 240 - lowoffs;
    int state = 0;

    void Hide();
    void Show();
};