#pragma once

#include <stdint.h>

#include <engine/Packets.h>

#define MAX_NETCLIENT 64

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
    uint8_t ipv4[4];
    char username[ENGINE_PACKET_MAXPLAYERNAME];
    state_e state = NETCLIENT_FREE;
};
}