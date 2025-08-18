#include <stdio.h>

#include <argparse/argparse.hpp>

#include <engine/cl_Client.h>
#include <engine/Console.h>
#include <engine/sv_Server.h>
#include <game/Game.h>

int Server(void)
{
    int res;
    engine::sv::Server server;

    engine::Console::LaunchTerm();
    server.Setup();
    Game::HookServer(&server);
    res = server.Run();
    engine::Console::KillTerm();
    return res;
}

int Client(void)
{
    int res;
    engine::cl::Client client;

    engine::Console::LaunchTerm();
    client.Setup();
    Game::HookClient(&client);
    res = client.Run();
    engine::Console::KillTerm();
    return res;
}

int main(int argc, char** argv)
{
    argparse::ArgumentParser parser;

    parser.add_argument("-s", "--server").flag();

    try
    {
        parser.parse_args(argc, argv);
    }
    catch(const std::exception& err)
    {
        fprintf(stderr, "%s\n", err.what());
        std::cerr << parser;
        return 1;
    }

    if(parser["--server"] == true)
        return Server();
    return Client();
}