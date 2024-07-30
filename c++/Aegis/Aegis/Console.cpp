#include "Console.h"

#include "Game.h"

std::string Console::Execute(std::string cmd)
{
    int i;

	std::string key, val, result;
    Command c;

    size_t spacepos = cmd.find(' ');

    if (spacepos != std::string::npos)
    {
        key = cmd.substr(0, spacepos);
        val = cmd.substr(spacepos + 1);
    }
    else
    {
        key = cmd;
    }

    for (i = 0; i < NCOMMANDS; i++)
    {
        if (key == commandmap[i])
            break;
    }

    if (commands[i] == nullptr)
        return "Command \"" + key + "\"not found.";

    c.cmd = key;
    c.val = val;
    c.func = commands[i];

    result = c.Execute();
    printf("running command \"%s\"...\n%s\n", cmd.c_str(), result.c_str());
    return result;
}

std::string Console::CommandMap(std::string val)
{
    Game::GetGame().map.Load((Game::GetGame().gamedir + "/maps/" + val).c_str());
    return "Loaded map \"" + Game::GetGame().gamedir + "/maps/" + val + "\".";
}