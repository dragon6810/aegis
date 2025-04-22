#include "EntityZombie.h"

#include "WalkAgent.h"

void EntityZombie::Init(const std::unordered_map <std::string, std::string>& pairs)
{
    EntityStudio::Init(pairs);

    this->navagent = std::make_unique<WalkAgent>(WalkAgent(&Game::GetGame().world));
}

void EntityZombie::Render(void)
{
    EntityStudio::Render();

    this->navagent->RenderPath();
}

void EntityZombie::Tick(void)
{
    EntityStudio::Tick();

    this->navagent->ConstructPath(this->pos, Vector3());
}

std::string EntityZombie::GetModelName(void)
{
    return "models/zgrunt/zgrunt";
}
