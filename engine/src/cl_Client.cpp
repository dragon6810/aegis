#include <engine/cl_Client.h>

#include <utilslib.h>

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
        case SDL_EVENT_KEY_DOWN:
            KeyDown(event.key.scancode);
            break;
        case SDL_EVENT_KEY_UP:
            KeyUp(event.key.scancode);
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
    uint64_t lastframe, thisframe;
    float frametime;

    Init();

    this->MakeWindow();
    lastframe = 0;
    while(!this->lastframe)
    {
        thisframe = TIMEMS;
        if(!lastframe)
            lastframe = thisframe;
        frametime = (float) (thisframe - lastframe) / 1000.0;

        this->PollWindow();
        Console::ExecTerm();

        this->player.ParseCmd(this->pinput->GenerateCmd());
        this->player.Move(frametime);
        Console::Print("Player position: ( %f %f ).\n", this->player.pos[0], this->player.pos[1]);

        lastframe = thisframe;
    }
    this->DestroyWindow();

    return 0;
}