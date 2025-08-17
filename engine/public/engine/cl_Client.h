#pragma once

#include <memory>

#include <SDL3/SDL.h>

#include <engine/cl_DumbClient.h>
#include <engine/cl_PlayerInput.h>
#include <engine/NetChan.h>
#include <engine/Packets.h>

#include "Player.h"

namespace engine::cl
{
    class Client
    {
    public:
        typedef struct gamestate_s
        {
            int32_t sequence;
            
            DumbClient svclients[MAX_PLAYER];
            packet::clsv_playercmd_t cmd;
            uint64_t senttime;
            uint64_t recievedtime;
        } gamestate_t;
    private:
        SDL_Window *win = NULL;
    private:
        void MakeWindow(void);
        void PollWindow(void);
        void DestroyWindow(void);
        void DrawClients(SDL_Renderer* render);
        void SendPackets(void);
        void Connect(void);
        void ConnectStr(const std::string& str);
        void ConnectCmd(const std::vector<std::string>& args);
        bool ProcessPacket(void);
        void ProcessHandshakeResponse(void);
        void ProcessRecieved(void);
        void RecordInput(void);
        void PredictLocal(void);
        void Init(void);
        void Cleanup(void);
    public:
        bool islastframe = false;
        float frametime;

        bool tryconnect = false;
        int clport;
        uint64_t connectstart;

        bool connected = false;
        NetChan netchan = NetChan();
        int clientid = -1;
        // these have prediction applied, unlike the ones in the current gametate in states[].
        DumbClient svclients[MAX_PLAYER];

        gamestate_t states[STATE_WINDOW];

        // games: set this to your own input class if you want to.
        std::unique_ptr<PlayerInput> pinput;

        void Setup(void);
        int Run(void);
    };
}