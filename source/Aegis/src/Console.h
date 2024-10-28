#pragma once

#include "ResourceManager.h"

class Console
{
public:
    void Load();
    void Render();

    void Toggle();
    void KeyInput(char c);

    bool IsDown();
private:
    bool isdown;

    ResourceManager::texture_t* consback;

    void Hide();
    void Show();
};