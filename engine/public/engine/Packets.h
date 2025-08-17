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
    TYPE_PLAYERCMD,
    TYPE_SVCL_PLAYERSTATE,
} type_e;

static const char clsv_handshake_message[] = "aegis handshake";

#pragma pack(push, 1)
typedef struct clsv_handshake_s
{
    char message[sizeof(clsv_handshake_message)];
    uint32_t version; // PROTOCOL_VERSION
    uint16_t port; // the what port to talk to the client through
    char username[ENGINE_PACKET_MAXPLAYERNAME];
} clsv_handshake_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct svcl_handshake_s
{
    char message[sizeof(clsv_handshake_message)];
    uint32_t version; // PROTOCOL_VERSION
    uint8_t clid; // the numerical id of the new client
} svcl_handshake_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct clsv_playercmd_s
{
    uint16_t type;
    uint16_t len;

    uint16_t time; // time in ms the command was down for. potential cheating opportunity?
    uint8_t move; // bits are 0 0 0 0 left right back forward
} clsv_playercmd_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct svcl_playerstate_s
{
    uint16_t type;
    uint16_t len;

    uint8_t id; // id of the client in question
    float x;
    float y;
} svcl_playerstate_t;
#pragma pack(pop)

};