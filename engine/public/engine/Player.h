#pragma once

#include <Eigen/Dense>

#include "Packets.h"

namespace engine
{
    class Player
    {
    public:
        Eigen::Vector2f pos = Eigen::Vector2f::Zero();
        Eigen::Vector2f wishdir = Eigen::Vector2f::Zero();

        void ParseCmd(packet::playercmd_t cmd);
        void Move(float frametime);
    };
}