#pragma once

#include "Packets.h"

namespace engine::cl
{
    // if you're a game, you can override this.
    // this stores pointers to itself, DO NOT MAKE DANGLING. SEGAULT WILL ENSUE.
    class PlayerInput
    {
    private:
        virtual void InitClient(void);
    public:
        bool forward;
        bool back;
        bool left;
        bool right;

        virtual packet::clsv_playercmd_t GenerateCmd(void);

        void Init(void);
    };
}