#pragma once

namespace engine::sv
{
    class Server
    {
    public:
        static constexpr int tickrate = 64;
        static constexpr float ticktime = 1.0 / (float) tickrate;
        static constexpr float tickms = ticktime * 1000;
    public:
        static Server* GetServer();
    public:
        int socket;

        static void sv_socketinfo(const std::vector<std::string>& args);

        void Setup(void);
        int Run(void);
    };
}