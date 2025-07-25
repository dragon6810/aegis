#pragma once

#include "EntityBase.h"

#include <mathlib.h>

#include "Camera.h"

class EntityCamera : public EntityBase
{
public:
	void Init(const std::unordered_map <std::string, std::string>& pairs) override;
    void Render(void) override;
    void Tick(void) override;

	void UpdateMouse(float x, float y);

	void SetUpGL(void);

	
	Camera cam;
	Eigen::Vector3f pos;
	Eigen::Vector3f rot;
	Eigen::Vector3f mousedir;

	float vfov = DEG2RAD(60.0);
	float aspect = 4.0 / 3.0;
};
