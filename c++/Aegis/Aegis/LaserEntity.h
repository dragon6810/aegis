#pragma once

#include "BaseEntity.h"

class LaserEntity : public BaseEntity
{
public:
	LaserEntity(BSPMap& map);

	virtual void Render() override;

	std::string target;
	BaseEntity* targetentity = nullptr;

	float width = 25.5;
private:
	void FindTarget();
};

