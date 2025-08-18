#include <game/Game.h>

#include <stdio.h>

void Game::HookClient(engine::cl::Client *cl)
{
    printf("hooking grunge into client.\n");
}

void Game::HookServer(engine::sv::Server *sv)
{
    printf("hooking grunge into server.\n");
}