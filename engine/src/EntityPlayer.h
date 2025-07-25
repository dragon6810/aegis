#pragma once

#include "EntityStudio.h"

#include <mathlib.h>

class EntityPlayer : public EntityStudio
{
public:
    void Render(void) override;
    void Tick(void) override;
    std::string GetModelName(void) override;
private:
    void SetTargetAngle(void);

    const float friction = 0.9 * ENGINE_TICKDUR;
    const float maxaccel = RAD2DEG(1.5) * ENGINE_TICKDUR;
    const float maxvel = RAD2DEG(3) * ENGINE_TICKDUR;

    float yawvel = 0.0;
    float yawtarget = 0.0;
    float yaw = 0.0;
};
