#pragma once

#include "MonsterEntity.h"

class MonsterBarney : public MonsterEntity
{
public:

	virtual void Init() override;
	MonsterBarney(BSPMap& map);
};

