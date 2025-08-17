#pragma once

#include <stdint.h>

#include <engine/NetChan.h>
#include <engine/Packets.h>
#include <engine/Player.h>

namespace engine::sv
{
class NetClient
{
public:
    typedef enum
    {
        NETCLIENT_CONNECTED=0,
        NETCLIENT_FREE,
    } state_e;
public:
    state_e state = NETCLIENT_FREE;
    NetChan netchan = NetChan();
    
    Player player;
    char username[ENGINE_PACKET_MAXPLAYERNAME];
};
}