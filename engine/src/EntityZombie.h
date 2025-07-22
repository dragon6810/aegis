#pragma once

#include "EntityStudio.h"

#include "NavAgent.h"

class EntityZombie : public EntityStudio
{
public:
    virtual void Init(const std::unordered_map <std::string, std::string>& pairs) override;
    virtual void Render(void) override;
    virtual void Tick(void) override;
    virtual std::string GetModelName(void) override;
protected:
    std::unique_ptr<NavAgent> navagent;
};