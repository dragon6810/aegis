#include <engine/Player.h>

void engine::Player::ParseCmd(packet::playercmd_t cmd)
{
    Eigen::Vector2f rightvec, forwardvec;

    rightvec = Eigen::Vector2f::UnitX();
    forwardvec = Eigen::Vector2f::UnitY();

    this->wishdir = Eigen::Vector2f::Zero();
    if(cmd.move & 0x8)
        this->wishdir -= rightvec;
    if(cmd.move & 0x4)
        this->wishdir += rightvec;
    if(cmd.move & 0x2)
        this->wishdir -= forwardvec;
    if(cmd.move & 0x1)
        this->wishdir += forwardvec;
}

void engine::Player::Move(float frametime)
{
    const float movespeed = 320.0;

    this->pos += this->wishdir * movespeed * frametime;
}