#pragma once

#include "EntityBase.h"

#include "Quaternion.h"

class EntityCamera : public EntityBase
{
public:
	void Init(const std::unordered_map <std::string, std::string>& pairs) override;
    void Render(void) override;

	void UpdateMouse(float x, float y);

	void SetUpGL(void);

	Vector3 pos;
	Vector3 rot;
	Vector3 mousedir;

	float vfov = 60.0 * DEG2RAD;
	float aspect = 4.0 / 3.0;
};
