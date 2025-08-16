#include "cl_Input.h"

#include <engine/Console.h>

std::unordered_map<short, std::string> engine::cl::keymapping;

void BindCmd(const std::vector<std::string>& args)
{
    printf("TODO: bind\n");
}

void engine::cl::InputInit(void)
{
    Console::RegisterCCmd( { "bind", BindCmd } );
}