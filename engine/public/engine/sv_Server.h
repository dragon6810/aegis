#pragma once

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
    public:
        static Server* GetServer();
    private:
        void InitNet(void);
        void Init(void);
        void Cleanup(void);
        void ProcessRecieved(void);
    public:
        void Setup(void);
        int Run(void);
    };
}