#pragma once

#include <engine/cl_Client.h>
#include <engine/sv_Server.h>

class Game
{
public:
    static void HookClient(engine::cl::Client *cl);
    static void HookServer(engine::sv::Server *sv);
};