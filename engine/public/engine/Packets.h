#pragma once

#include <stdint.h>

#define ENGINE_PACKET_PROTOCOL_VERSION 2025
#define ENGINE_PACKET_MAXPLAYERNAME 32

namespace engine::packet
{
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
    uint8_t move; // bits are 0 0 0 0 left right back forward
} clsv_playercmd_t;
};