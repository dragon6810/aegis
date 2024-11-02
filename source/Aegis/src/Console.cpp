#include "Console.h"

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdarg>
#include <algorithm>

#include "Wad.h"
#include "Game.h"

void Console::Print(const char* format, ...)
{
    va_list args;
    char buffer[1024];

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    printf("%s", buffer);

    Game::GetGame().console.output.append(buffer);
}

Console::~Console()
{
    ResourceManager::AbandonTexture(consback);
    ResourceManager::AbandonTexture(font.tex);
}

bool Console::IsDown()
{
    return isdown;
}

void Console::KeyInput(char c)
{
    if (state != 2)
        return;

    Console::Print("Console input: '%c'.\n", c);
}

void Console::Hide()
{
    Console::Print("Hide console.\n");
    isdown = false;
    state = 3;
}

void Console::Show()
{
    Console::Print("Show console.\n");
    isdown = true;
    state = 1;
}

void Console::Toggle()
{
    if(IsDown())
        Hide();
    else
        Show();
}

void Console::Render()
{
    int i;

    int nrows;
    std::string curline;
    char* c;

    if(!state)
        return;

    switch (state)
    {
    case 1:
        curoffs -= (float)(visheight << 1) * Game::GetGame().deltatime;

        if (curoffs <= lowoffs)
        {
            curoffs = lowoffs;
            state = 2;
        }
        break;
    case 3:
        curoffs += (float)(visheight << 1) * Game::GetGame().deltatime;

        if (curoffs >= 240)
        {
            curoffs = 240;
            state = 0;
        }
        break;
    default:
        break;
    }

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    if(consback)
        glBindTexture(GL_TEXTURE_2D, consback->name);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex2f(0, 0 + (int)curoffs);
    glTexCoord2f(0, 0);
    glVertex2f(0, 240 + (int)curoffs);
    glTexCoord2f(1, 0);
    glVertex2f(320, 240 + (int)curoffs);
    glTexCoord2f(1, 1);
    glVertex2f(320, 0 + (int)curoffs);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5f);

    nrows = visheight / font.tex->height - 1;
    c = &output[output.size() - 1];
    if (*c == '\n')
        c--;
    i = 0;
    while (i < nrows && c >= output.data())
    {
        curline = "";
        while (*c != '\n' && c >= output.data())
        {
            curline.push_back(*c);
            c--;
        }

        std::reverse(curline.begin(), curline.end());
        font.DrawText(curline, 0, (int) curoffs + (i + 2) * font.tex->height);
        i++;
        c--;
    }

    glDisable(GL_ALPHA_TEST);
}

void Console::Load()
{
    Wad wad;

    wad.Open("aegis.wad");
    consback = wad.LoadTexture("CONSBACK");
    font = wad.LoadFont("grit");
    wad.Unload();

    ResourceManager::UseTexture(consback);
    ResourceManager::UseTexture(font.tex);
}