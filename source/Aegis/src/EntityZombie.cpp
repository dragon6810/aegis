#include "EntityZombie.h"

void EntityPlayer::Render(void)
{
    EntityStudio::Render();
}

void EntityPlayer::Tick(void)
{
    EntityStudio::Tick();

    SetTargetAngle();
}

std::string EntityPlayer::GetModelName(void)
{
    return "models/zgrunt.mdl";
}
