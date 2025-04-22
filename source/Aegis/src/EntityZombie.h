#pragma once

#include "EntityStudio.h"

class EntityZombie : public EntityStudio
{
public:
    void Render(void) override;
    void Tick(void) override;
    std::string GetModelName(void) override;
private:
    
};