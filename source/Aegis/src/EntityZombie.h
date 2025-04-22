#pragma once

#include "EntityStudio.h"

class EntityPlayer : public EntityStudio
{
public:
    void Render(void) override;
    void Tick(void) override;
    std::string GetModelName(void) override;
private:
    void SetTargetAngle(void);

    const float friction = 0.9 * ENGINE_TICKDUR;
    const float maxaccel = 1.5 * ENGINE_TICKDUR * RAD2DEG;
    const float maxvel = 3 * ENGINE_TICKDUR * RAD2DEG;

    float yawvel = 0.0;
    float yawtarget = 0.0;
    float yaw = 0.0;
};