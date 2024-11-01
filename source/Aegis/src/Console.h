#pragma once

#include "ResourceManager.h"
#include "RasterFont.h"

class Console
{
public:
    ~Console();

    void Load();
    void Render();

    void Toggle();
    void KeyInput(char c);

    bool IsDown();
private:
    bool isdown;

    ResourceManager::texture_t* consback;
    RasterFont font;

    void Hide();
    void Show();
};