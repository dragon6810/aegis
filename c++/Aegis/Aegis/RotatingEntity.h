#pragma once

#include "BSPModelEntity.h"

#define FUNC_ROTATING_FLAGS_START_ON              1
#define FUNC_ROTATING_FLAGS_REVERSE_DIRECTION     2
#define FUNC_ROTATING_FLAGS_X_AXIS                4
#define FUNC_ROTATING_FLAGS_Y_AXIS                8
#define FUNC_ROTATING_FLAGS_ACC_DCC              16
#define FUNC_ROTATING_FLAGS_FAN_PAIN             32
#define FUNC_ROTATING_FLAGS_NOT_SOLID            64
#define FUNC_ROTATING_FLAGS_SMALL_SOUND_RADIUS  128
#define FUNC_ROTATING_FLAGS_MEDIUM_SOUND_RADIUS 256
#define FUNC_ROTATING_FLAGS_LARGE_SOUND_RADIUS  512

class RotatingEntity : public BSPModelEntity
{
public:
    RotatingEntity(BSPMap& map);

    virtual void Init() override;
    virtual void Think(float deltatime) override;
    
    void SetSpeed(float speed);
protected:
    vec3_t rotationalvelocity = { 0.0, 0.0, 0.0 };
    float speed = 0.0;
    float friction = 0.0;
    float speedpercent = 0.0;
};

