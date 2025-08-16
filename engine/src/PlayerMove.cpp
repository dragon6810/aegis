#include "PlayerMove.h"

void engine::PlayerMove::Init(bool client)
{
    if(client)
        InitClient();
    else
        InitServer();
}

void engine::PlayerMove::InitClient(void)
{

}

void engine::PlayerMove::InitServer(void)
{

}