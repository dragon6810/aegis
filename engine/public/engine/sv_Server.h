#pragma once

#include <engine/sv_NetClient.h>
#include <engine/Packets.h>

namespace engine::sv
{
    class Server
    {
    public:
        static constexpr int tickrate = 64;
        static constexpr float ticktime = 1.0 / (float) tickrate;
        static constexpr float tickms = ticktime * 1000;
    private:
        int clsocket = -1;
    public:
        bool lasttick = false;
        int nclients = 0;
        NetClient clients[MAX_PLAYER];
    public:
        static Server* GetServer();
    private:
        void InitNet(void);
        void Init(void);
        void Cleanup(void);
        void ProcessHandshake(const packet::clsv_handshake_t* packet, const uint8_t addr[4]);
        void ProcessClientPacket(int icl, const void* data, int datalen);
        void ProcessRecieved(void);
        void SendPackets(void);
    public:
        // returns -1 if no client
        int ClientByAddr(const uint8_t addr[4]);

        // returns -1 if full
        int FindFreeClient(void);

        void Setup(void);
        int Run(void);
    };
}