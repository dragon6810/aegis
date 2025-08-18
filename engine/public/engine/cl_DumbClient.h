#pragma once

#include <engine/Player.h>

namespace engine::cl
{
    class DumbClient
    {
    public:
        typedef enum
        {
            STATE_CONNECTED=0,
            STATE_FREE
        } state_e;
    public:
        state_e state = STATE_FREE;
        Player player = Player();
    };
};