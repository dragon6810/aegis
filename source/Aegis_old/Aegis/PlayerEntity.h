#pragma once

#include "BaseEntity.h"

#include "SModel.h"

class PlayerEntity : public BaseEntity
{
public:
	PlayerEntity(BSPMap& map);

	virtual void Think(float deltatime) override;
	virtual void Render() override;
private:
	SModel studio;

	float lastlastlookdir = 0, lastlookdir = 0, lookdir = 0;
	float yawvel;
};

