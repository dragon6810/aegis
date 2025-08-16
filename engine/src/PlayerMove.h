#pragma once

namespace engine
{
    class PlayerMove
    {
    public:
        void Init(bool client);
    private:
        void InitClient(void);
        void InitServer(void);
    };
}