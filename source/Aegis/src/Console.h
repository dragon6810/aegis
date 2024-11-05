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
    void DecCursor();
    void IncCursor();
    void Backspace();
    void Delete();

    bool IsDown();
private:
    bool isdown;

    std::string input;
    int cursor;
    int window;

    ResourceManager::texture_t* consback;
    RasterFont font;
    float curoffs = 240;
    const int lowoffs = 120; // So theres not a little nibble of characters at the top
    const int visheight = 240 - lowoffs;
    int state = 0;

    void Hide();
    void Show();
};