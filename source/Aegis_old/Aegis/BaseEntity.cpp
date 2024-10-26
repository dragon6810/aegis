#include "BaseEntity.h"

BaseEntity::BaseEntity(BSPMap& map)
{
	this->map = &map;
}

void BaseEntity::Init()
{

}

void BaseEntity::Think(float deltatime)
{
	return;
}

void BaseEntity::Render()
{
	return;
}

void BaseEntity::On()
{
	this->on = true;
}

void BaseEntity::Off()
{
	this->on = false;
}

void BaseEntity::Toggle()
{
	if (on)
		Off();
	else
		On();
}

rayhitinfo_t BaseEntity::RayCollides(collisionray_t ray)
{
	return { false, {0.0, 0.0, 0.0} };
}

void BaseEntity::SetMap(BSPMap& map)
{
	this->map = &map;
}