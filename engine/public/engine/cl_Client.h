#pragma once

#include <memory>

#include <SDL3/SDL.h>

#include <engine/cl_PlayerInput.h>
#include <engine/NetChan.h>

#include "Player.h"

namespace engine::cl
{
    class Client
    {
    private:
        SDL_Window *win = NULL;
    private:
        void MakeWindow(void);
        void PollWindow(void);
        void DestroyWindow(void);
        void DrawClients(SDL_Renderer* render);
        void Connect(const uint8_t svaddr[4], int port);
        void ConnectStr(const std::string& str);
        void ConnectCmd(const std::vector<std::string>& args);
        void TryConnection(void);
        void Init(void);
        void Cleanup(void);
    public:
        bool lastframe = false;

        int svsocket = -1;
        
        bool tryconnect = false;
        uint8_t serveraddr[4];
        int serverport;
        uint64_t connectstart;

        // games: set this to your own input class if you want to.
        std::unique_ptr<PlayerInput> pinput;
        Player player;

        void Setup(void);
        int Run(void);
    };
}