#include "MonsterEntity.h"

#include "Game.h"

#include "Quaternion.h"

MonsterEntity::MonsterEntity(BSPMap& map) : BaseEntity(map)
{
    
}

void MonsterEntity::Init()
{
	model.Load(GetStudioPath().c_str());
	model.map = map;
}

void MonsterEntity::Render()
{
	model.camerapos = camerapos;

	glPushMatrix();

	glTranslatef(position.x, position.y, position.z);
	glRotatef(rotation.x * RAD2DEG, 1.0, 0.0, 0.0);
	glRotatef(rotation.y * RAD2DEG, 0.0, 1.0, 0.0);
	glRotatef(rotation.z * RAD2DEG, 0.0, 0.0, 1.0);

	model.render();

	glPopMatrix();
}

void MonsterEntity::Think(float deltatime)
{
	model.Tick();
}

std::string MonsterEntity::GetStudioPath()
{
    return Game::GetGame().gamedir + "/models/missingno/missingno.mdl";
}
