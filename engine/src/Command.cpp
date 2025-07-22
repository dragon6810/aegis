#include "Command.h"

#include "Game.h"
#include "Input.h"

#include "EntityStudio.h"

std::string Command::autoexec = "userdata/autoexec.cfg";
std::string Command::datadir = "deadbird/";
bool Command::filtertextures = false;

const std::unordered_map<std::string, bool(*)(std::string)> Command::cmdtable =
{
    { "+map", &Command::CommandMap },
    { "-map", &Command::CommandMap },
    {  "map", &Command::CommandMap },
    { "+bind", &Command::CommandBind },
    { "-bind", &Command::CommandBind },
    {  "bind", &Command::CommandBind },
    { "toggleconsole", &Command::CommandToggleConsole },
    { "togglestudio", &Command::CommandToggleStudio },
    { "togglestudioskeleton", &Command::CommandToggleStudioSkeletons },
};

bool Command::CommandMap(std::string val)
{
    Game::GetGame().world.Load(val);

    return true;
}

bool Command::CommandBind(std::string val)
{
    int i;
    std::string key, cmd;

    i = 0;
    while (i < val.size() && val[i] > 32)
    {
        key.push_back(val[i]);
        i++;
    }

    while (i < val.size() && val[i] <= 32)
        i++;

    while (i < val.size())
    {
        cmd.push_back(val[i]);
        i++;
    }

    if (Input::stringkeycodes.find(key) == Input::stringkeycodes.end())
    {
        Console::Print("Unknown key \"%s\".\n", key.c_str());
        return false;
    }

    i = Input::stringkeycodes.at(key);
    Input::keycommands[i] = cmd;

    return true;
}

bool Command::CommandToggleConsole(std::string val)
{
    Game::GetGame().console.Toggle();
    return true;
}

bool Command::CommandToggleStudio(std::string val)
{
    EntityStudio::drawstudio = !EntityStudio::drawstudio;
    return true;
}

bool Command::CommandToggleStudioSkeletons(std::string val)
{
    EntityStudio::drawskeleton = !EntityStudio::drawskeleton;
    return true;
}

void Command::Run(std::string key, std::string val)
{
    if (cmdtable.find(key) == cmdtable.end())
    {
        Console::Print("Command not found \"%s\".\n", key.c_str());
        return;
    }

    if(val != "")
        Console::Print("Running command \"%s\".\n", (key + " " + val).c_str());
    else
        Console::Print("Running command \"%s\".\n", key.c_str());
    cmdtable.at(key)(val);
}