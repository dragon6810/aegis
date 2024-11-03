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

    if(c == '\n')
    {
        Game::GetGame().ParseCommands(input);
        input.clear();
        cursor = 0;
        window = 0;
        return;
    }

    if(cursor == input.size())
        cursor++;

    input.push_back(c);
}

void Console::DecCursor()
{
    if(cursor)
        cursor--;
}

void Console::IncCursor()
{
    if(cursor < input.size())
        cursor++;
}

void Console::Backspace()
{
    if(!cursor)
        return;
    
    if(input.size())
        input.erase(input.begin() + cursor - 1);
    
    cursor--;
}

void Console::Delete()
{
    if(cursor < input.size())
        input.erase(input.begin() + cursor);
}

void Console::Hide()
{
    isdown = false;
    state = 3;
}

void Console::Show()
{
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
    int cursorw, cursorx;
    long long now;

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

    font.DrawText(input, 0, (int) curoffs);

    if(cursor < input.size())
        cursorw = font.widths[input[cursor]];
    else
        cursorw = 5;

    for(i=0, cursorx=0; i<cursor && i<input.size(); i++)
        cursorx += font.widths[input[i]];

    if(cursor)
        cursorx += cursor;

    now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    if((now >> 10) & 1)
    {
        glBegin(GL_QUADS);
        glVertex2f(cursorx, curoffs);
        glVertex2f(cursorx + cursorw, curoffs);
        glVertex2f(cursorx + cursorw, curoffs + font.tex->height);
        glVertex2f(cursorx, curoffs + font.tex->height);
        glEnd();
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