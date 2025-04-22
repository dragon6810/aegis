#include "EntityZombie.h"

void EntityZombie::Render(void)
{
    EntityStudio::Render();
}

void EntityZombie::Tick(void)
{
    EntityStudio::Tick();
}

std::string EntityZombie::GetModelName(void)
{
    return "models/zgrunt/zgrunt";
}
