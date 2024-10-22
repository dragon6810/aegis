//
//  EntityBullet.hpp
//  Aegis
//
//  Created by Henry Dunn on 10/22/24.
//

#pragma once

#include "BaseEntity.h"

class EntityBullet : public BaseEntity
{
public:
    virtual void Render() override;
    virtual void Think(float deltatime) override;
    virtual void Init() override;
    EntityBullet(BSPMap& map);
    
    void Start(vec3_t startpos, vec3_t endpos, float speed);
    
    float radius = 0.2;
    vec3_t direction;
    float distance;
    float progress;
};
