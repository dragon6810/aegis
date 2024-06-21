#include "MonsterEntity.h"

#include "Quaternion.h"

MonsterEntity::MonsterEntity(BSPMap& map) : BaseEntity(map)
{

}

void MonsterEntity::Render()
{
	model.camerapos = camerapos;

	glPushMatrix();

	glTranslatef(position.x, position.y, position.z);
	glRotatef(rotation.z * RAD2DEG, 0.0, 0.0, 1.0);
	glRotatef(rotation.y * RAD2DEG, 0.0, 1.0, 0.0);
	glRotatef(rotation.x * RAD2DEG, 1.0, 0.0, 0.0);

	model.render();

	glPopMatrix();
}

void MonsterEntity::Think(float deltatime)
{
	model.Tick();
}