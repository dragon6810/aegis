//
//  GruntEntity.cpp
//  Aegis
//
//  Created by Henry Dunn on 8/17/24.
//

#include "GruntEntity.h"

#include "Game.h"

GruntEntity::GruntEntity(BSPMap& map) : MonsterEntity(map)
{
    
}

std::string GruntEntity::GetStudioPath()
{
    return Game::GetGame().gamedir + "/models/zgrunt/zgrunt.mdl";
}

int GruntEntity::GetPathMode()
{
    return PATH_DOOM;
}

int GruntEntity::GetClippingHull()
{
    return ZOMBIE_HULL;
}

float GruntEntity::GetMaxSpeed()
{
    return 64.0 * ENGINE_TICKDUR;
}
