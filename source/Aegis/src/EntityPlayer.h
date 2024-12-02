#pragma once

#include "EntityStudio.h"

class EntityPlayer : public EntityStudio
{
public:
    void XRender(void) override;
    void XTick(void) override;
    std::string GetModelName(void) override;
private:
    void SetTargetAngle(void);
};
