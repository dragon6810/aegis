#include "EntityPlayer.h"

void EntityPlayer::XRender(void)
{
    // Nothing
}

void EntityPlayer::XTick(void)
{
    controllers[0].cur += 1.0f;
}

std::string EntityPlayer::GetModelName(void)
{
    return "models/tank";
}
