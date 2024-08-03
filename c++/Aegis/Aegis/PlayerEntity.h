#pragma once

#include "BaseEntity.h"

#include "SModel.h"

class PlayerEntity : public BaseEntity
{
public:
	PlayerEntity(BSPMap& map);

	virtual void Render() override;
private:
	SModel studio;

	float lookdir;
};

