#pragma once

#include <unordered_map>

#include <SDL3/SDL.h>

namespace engine::cl
{
extern std::unordered_map<std::string, std::string> keymapping; // name to CCmd name

void InputInit(void);
void KeyDown(SDL_Scancode scancode);
void KeyUp(SDL_Scancode scancode);
};