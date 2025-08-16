#include <engine/cl_Client.h>

#include <engine/Console.h>

#include "cl_Input.h"

void engine::cl::Client::Init(void)
{
    InputInit();
    this->pinput->Init();
}

void engine::cl::Client::Setup(void)
{
    this->pinput = std::make_unique<PlayerInput>(PlayerInput());
}

int engine::cl::Client::Run(void)
{
    Init();

    while(1)
    {
        Console::ExecTerm();
    }

    return 0;
}