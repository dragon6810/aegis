#pragma once

#include "EntityStudio.h"

class EntityPlayer : public EntityStudio
{
public:
    std::string GetModelName(void) override;
};
