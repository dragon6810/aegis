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
