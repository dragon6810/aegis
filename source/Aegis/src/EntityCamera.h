#pragma once

#include "EntityBase.h"

class EntityCamera : public EntityBase
{
public:
	void Init(const std::unordered_map <std::string, std::string>& pairs) override;

	Vector3 pos;
	Vector3 rot;
};