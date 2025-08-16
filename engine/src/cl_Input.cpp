#include "cl_Input.h"

#include <SDL3/SDL.h>

#include <engine/Console.h>

std::unordered_map<SDL_Scancode, std::string> scancodestr = 
{
    { SDL_SCANCODE_A, "a", },
    { SDL_SCANCODE_B, "b", },
    { SDL_SCANCODE_C, "c", },
    { SDL_SCANCODE_D, "d", },
    { SDL_SCANCODE_E, "e", },
    { SDL_SCANCODE_F, "f", },
    { SDL_SCANCODE_G, "g", },
    { SDL_SCANCODE_H, "h", },
    { SDL_SCANCODE_I, "i", },
    { SDL_SCANCODE_J, "j", },
    { SDL_SCANCODE_K, "k", },
    { SDL_SCANCODE_L, "l", },
    { SDL_SCANCODE_M, "m", },
    { SDL_SCANCODE_N, "n", },
    { SDL_SCANCODE_O, "o", },
    { SDL_SCANCODE_P, "p", },
    { SDL_SCANCODE_Q, "q", },
    { SDL_SCANCODE_R, "r", },
    { SDL_SCANCODE_S, "s", },
    { SDL_SCANCODE_T, "t", },
    { SDL_SCANCODE_U, "u", },
    { SDL_SCANCODE_V, "v", },
    { SDL_SCANCODE_W, "w", },
    { SDL_SCANCODE_X, "x", },
    { SDL_SCANCODE_Y, "y", },
    { SDL_SCANCODE_Z, "z", },

    { SDL_SCANCODE_1, "1", },
    { SDL_SCANCODE_2, "2", },
    { SDL_SCANCODE_3, "3", },
    { SDL_SCANCODE_4, "4", },
    { SDL_SCANCODE_5, "5", },
    { SDL_SCANCODE_6, "6", },
    { SDL_SCANCODE_7, "7", },
    { SDL_SCANCODE_8, "8", },
    { SDL_SCANCODE_9, "9", },
    { SDL_SCANCODE_0, "0", },

    { SDL_SCANCODE_RETURN, "return", },
    { SDL_SCANCODE_ESCAPE, "escape", },
    { SDL_SCANCODE_BACKSPACE, "backspace", },
    { SDL_SCANCODE_TAB, "tab", },
    { SDL_SCANCODE_SPACE, "space", },

    { SDL_SCANCODE_MINUS, "-", },
    { SDL_SCANCODE_EQUALS, "=", },
    { SDL_SCANCODE_LEFTBRACKET, "[", },
    { SDL_SCANCODE_RIGHTBRACKET, "]", },
    { SDL_SCANCODE_BACKSLASH, "\\", },
    { SDL_SCANCODE_SEMICOLON, ";", },
    { SDL_SCANCODE_APOSTROPHE, "'", },
    { SDL_SCANCODE_GRAVE, "`", },
    { SDL_SCANCODE_COMMA, ",", },
    { SDL_SCANCODE_PERIOD, ".", },
    { SDL_SCANCODE_SLASH, "/", },

    { SDL_SCANCODE_CAPSLOCK, "capslock", },

    { SDL_SCANCODE_F1, "F1", },
    { SDL_SCANCODE_F2, "F2", },
    { SDL_SCANCODE_F3, "F3", },
    { SDL_SCANCODE_F4, "F4", },
    { SDL_SCANCODE_F5, "F5", },
    { SDL_SCANCODE_F6, "F6", },
    { SDL_SCANCODE_F7, "F7", },
    { SDL_SCANCODE_F8, "F8", },
    { SDL_SCANCODE_F9, "F9", },
    { SDL_SCANCODE_F10, "F10", },
    { SDL_SCANCODE_F11, "F11", },
    { SDL_SCANCODE_F12, "F12", },

    { SDL_SCANCODE_RIGHT, "right", },
    { SDL_SCANCODE_LEFT, "left", },
    { SDL_SCANCODE_DOWN, "down", },
    { SDL_SCANCODE_UP, "up", },

    { SDL_SCANCODE_LSHIFT, "shift", },
    { SDL_SCANCODE_LCTRL, "ctrl", },
    { SDL_SCANCODE_LALT, "alt", },
    { SDL_SCANCODE_LGUI, "meta", },
    { SDL_SCANCODE_RSHIFT, "shift", },
    { SDL_SCANCODE_RCTRL, "ctrl", },
    { SDL_SCANCODE_RALT, "alt", },
    { SDL_SCANCODE_RGUI, "meta", },
};

std::unordered_map<std::string, std::string> engine::cl::keymapping;

void BindCmd(const std::vector<std::string>& args)
{
    std::unordered_map<SDL_Scancode, std::string>::iterator it;

    if(args.size() != 2 && args.size() != 3)
    {
        engine::Console::Print("expected exactly 1 or 2 args to bind.\n");
        return;
    }

    for(it=scancodestr.begin(); it!=scancodestr.end(); it++)
        if(it->second == args[1])
            break;
    if(it == scancodestr.end())
    {
        engine::Console::Print("unknown input key \"%s\".\n", args[1].c_str());
    }

    if(args.size() == 2)
    {
        if(engine::cl::keymapping.find(args[1]) != engine::cl::keymapping.end())
            engine::Console::Print("%s is bound to \"%s\".\n", args[1].c_str(), engine::cl::keymapping[args[1]].c_str());
        else
            engine::Console::Print("%s is not bound.\n", args[1].c_str());
        return;
    }

    engine::cl::keymapping[args[1]] = args[2];
}

void engine::cl::InputInit(void)
{
    Console::RegisterCCmd( { "bind", BindCmd } );
}