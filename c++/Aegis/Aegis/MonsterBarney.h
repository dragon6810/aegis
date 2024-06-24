#pragma once

#include "MonsterEntity.h"

class MonsterBarney : public MonsterEntity
{
public:
	virtual void Init() override;
	virtual void Think(float deltatime) override;
	MonsterBarney(BSPMap& map);
};

