#pragma once

#include "BaseEntity.h"

#include "SModel.h"

class MonsterEntity : public BaseEntity
{
public:
	virtual void Render() override;
	virtual void Think(float deltatime) override;
	MonsterEntity(BSPMap& map);
protected:
	SModel model;
};

