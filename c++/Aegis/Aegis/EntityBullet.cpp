//
//  EntityBullet.cpp
//  Aegis
//
//  Created by Henry Dunn on 10/22/24.
//

#include "EntityBullet.h"
#include "mathutils.h"

EntityBullet::EntityBullet(BSPMap& map) : BaseEntity(map)
{

}

void EntityBullet::Render()
{
    
}

void EntityBullet::Think(float deltatime)
{
    
}

void EntityBullet::Init()
{
    
}

void EntityBullet::Start(vec3_t startpos, vec3_t endpos, float speed)
{
    position = startpos;
    distance = Vector3Length(endpos - startpos);
    direction = NormalizeVector3(endpos - startpos);
    this->speed = speed;
}

float radius;
