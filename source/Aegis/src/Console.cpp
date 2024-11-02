#include "Console.h"

#include <stdio.h>

#include "Wad.h"

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
    printf("Console input: '%c'.\n", c);
}

void Console::Hide()
{
    printf("Hide console.\n");
    isdown = false;
}

void Console::Show()
{
    printf("Show console.\n");
    isdown = true;
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
    if(!isdown)
        return;

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    if(consback)
        glBindTexture(GL_TEXTURE_2D, consback->name);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex2f(0, 0);
    glTexCoord2f(0, 0);
    glVertex2f(0, 240);
    glTexCoord2f(1, 0);
    glVertex2f(320, 240);
    glTexCoord2f(1, 1);
    glVertex2f(320, 0);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    font.DrawText("Aegis Font Test\nThis is a new line", 0, 6);
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