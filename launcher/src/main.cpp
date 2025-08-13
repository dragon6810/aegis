#include <stdio.h>

#include <argparse/argparse.hpp>

#include <engine/cl_Client.h>
#include <engine/sv_Server.h>
#include <game/Game.h>

int Server(void)
{
    engine::sv::Server server;

    server.Setup();
    Game::HookServer(&server);
    return server.Run();
}

int Client(void)
{
    engine::cl::Client client;

    client.Setup();
    Game::HookClient(&client);
    return client.Run();
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