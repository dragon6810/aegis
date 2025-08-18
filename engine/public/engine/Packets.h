#pragma once

#include <stdint.h>

#define ENGINE_PACKET_PROTOCOL_VERSION 2025
#define ENGINE_PACKET_MAXPLAYERNAME 32

namespace engine::packet
{
/*
 * Packet Structure:
 * uint16_t type (type_e)
 * uint16_t length (length of payload only)
 * payload
 * 
 * NOTE: all packets should be big endian
*/

typedef enum
{
    TYPE_INVALIDPACKET=0,
    TYPE_HANDSHAKE,
    TYPE_PLAYERCMD,
    TYPE_SVCL_PLAYERSTATE,
} type_e;

static const char clsv_handshake_message[] = "aegis handshake";

#pragma pack(push, 1)
typedef struct clsv_playercmd_s
{
    uint16_t type;

    uint16_t time; // time in ms the command was down for. potential cheating opportunity?
    uint8_t move; // bits are 0 0 0 0 left right back forward
} clsv_playercmd_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct svcl_playerstate_s
{
    uint16_t type;

    uint8_t id; // id of the client in question
    float x;
    float y;
} svcl_playerstate_t;
#pragma pack(pop)

};