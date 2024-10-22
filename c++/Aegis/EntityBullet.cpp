//
//  EntityBullet.cpp
//  Aegis
//
//  Created by Henry Dunn on 10/22/24.
//

#include "EntityBullet.h"
#include "mathutils.h"
#include "Game.h"

EntityBullet::EntityBullet(BSPMap& map) : BaseEntity(map)
{
    
}

void EntityBullet::Render()
{
    glColor3f(0, 1, 0);
    glBegin(GL_POINTS);
    glVertex3f(position.x, position.y, position.z);
    glEnd();
    glColor3f(1, 1, 1);
}

void EntityBullet::Think(float deltatime)
{
    position = position + direction * (movespeed * ENGINE_TICKDUR);
}

void EntityBullet::Init()
{
    
}

void EntityBullet::Start(vec3_t startpos, vec3_t endpos, float speed)
{
    position = startpos;
    distance = Vector3Length(endpos - startpos);
    direction = NormalizeVector3(endpos - startpos);
    progress = 0;
    movespeed = speed;
}
