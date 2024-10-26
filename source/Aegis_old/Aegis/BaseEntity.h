#pragma once

#include "collision.h"
#include "BSPMap.h"

#include <string>

class BSPMap;

class BaseEntity
{
public:
	BaseEntity() {};
	BaseEntity(BSPMap& map);

	virtual void Init();
	virtual void Think(float deltatime);
	virtual void Render();
	virtual void On();
	virtual void Off();
	void Toggle();
	virtual rayhitinfo_t RayCollides(collisionray_t ray);

	void SetMap(BSPMap& map);

	vec3_t position = { 0.0, 0.0, 0.0 };
	vec3_t rotation = { 0.0, 0.0, 0.0 };
	vec3_t cameraforward = { 0.0, 0.0, 0.0 };
	vec3_t cameraup = { 0.0, 0.0, 0.0 };
	vec3_t camerapos = { 0.0, 0.0, 0.0 };

	std::string targetname;
	std::string classname;

	bool on;
	int flags = 0;
protected:
	BSPMap* map;
};