#pragma once

#include <stdint.h>

namespace engine::packet
{
typedef struct
{
    uint8_t move; // bits are 0 0 0 0 left right back forward
} playercmd_t;
};