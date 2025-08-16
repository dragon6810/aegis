#include <engine/cl_Client.h>

#include <engine/Console.h>

#include "cl_Input.h"

void engine::cl::Client::MakeWindow(void)
{
    this->win = SDL_CreateWindow("aegis", 640, 360, 0);
}

void engine::cl::Client::PollWindow(void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) 
    {
        switch(event.type)
        {
        case SDL_EVENT_QUIT:
            this->lastframe = true;
            break;
        default:
            break;
        }
    }
}

void engine::cl::Client::DestroyWindow(void)
{
    SDL_DestroyWindow(this->win);
    this->win = NULL;
}

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
    SDL_Event event;

    Init();

    this->MakeWindow();
    while(!this->lastframe)
    {
        this->PollWindow();
        Console::ExecTerm();
    }
    this->DestroyWindow();

    return 0;
}