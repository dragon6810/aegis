#include "RotatingEntity.h"

#include "Quaternion.h"

#include <GL/glew.h>

RotatingEntity::RotatingEntity(BSPMap& map) : BSPModelEntity(map)
{

}

void RotatingEntity::Init()
{
	if (flags & FUNC_ROTATING_FLAGS_START_ON)
		speedpercent = 100.0;
}

void RotatingEntity::Think(float deltatime)
{
	rotationalvelocity.z = -speed * speedpercent / 100.0;

	rotation.x += rotationalvelocity.x / 60.0 * deltatime;
	rotation.y += rotationalvelocity.y / 60.0 * deltatime;
	rotation.z += rotationalvelocity.z / 60.0 * deltatime;
}

void RotatingEntity::SetSpeed(float speed)
{
	this->speed = speed;
}