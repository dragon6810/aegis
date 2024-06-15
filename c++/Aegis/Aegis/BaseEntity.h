#pragma once

#include "collision.h"
#include "BSPMap.h"

class BSPMap;

class BaseEntity
{
public:
	BaseEntity(BSPMap& map);

	virtual void Think(float deltatime);
	virtual void Render();
	virtual void On();
	virtual void Off();
	void Toggle();
	virtual rayhitinfo_t RayCollides(collisionray_t ray);

	vec3_t position = { 0.0, 0.0, 0.0 };
	vec3_t rotation = { 0.0, 0.0, 0.0 };

	bool on;
protected:
	BSPMap* map;
};