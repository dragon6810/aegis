#include "Command.h"

const std::unordered_map<std::string, bool(*)(std::string)> Command::cmdtable =
{
    {"+map", &Command::CommandMap},
    {"-map", &Command::CommandMap},
    { "map", &Command::CommandMap},
};

bool Command::CommandMap(std::string val)
{
    printf("Load Map \"%s\".\n", val.c_str());

    return true;
}

void Command::Run(std::string key, std::string val)
{
    if (cmdtable.find(key) == cmdtable.end())
    {
        printf("Command not found \"%s\".\n", key.c_str());
        return;
    }

    printf("Running command \"%s\".\n", (key + " " + val).c_str());
    cmdtable.at(key)(val);
}