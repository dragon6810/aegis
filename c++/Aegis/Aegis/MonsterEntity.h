#pragma once

#include "BaseEntity.h"

#include "rendermodel.h"

class MonsterEntity : public BaseEntity
{
public:
	virtual void Render() override;
	MonsterEntity(BSPMap& map);
protected:
	SModel model;
};

