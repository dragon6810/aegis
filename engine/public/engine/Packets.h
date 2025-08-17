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
    TYPE_PLAYERCMD=1,
} type_e;

static const char clsv_handshake_message[] = "aegis handshake";

typedef struct clsv_handshake_s
{
    char message[sizeof(clsv_handshake_message)];
    uint32_t version; // PROTOCOL_VERSION
    char username[ENGINE_PACKET_MAXPLAYERNAME];
} clsv_handshake_t;

typedef struct svcl_handshake_s
{
    char message[sizeof(clsv_handshake_message)];
    uint32_t version; // PROTOCOL_VERSION
} svcl_handshake_t;

typedef struct clsv_playercmd_s
{
    uint16_t type;
    uint16_t len;

    uint16_t time; // time in ms the command was down for. potential cheating opportunity?
    uint8_t move; // bits are 0 0 0 0 left right back forward
} clsv_playercmd_t;
};