#pragma once
#include "WallEntity.h"
class IllusionaryEntity : public WallEntity
{
public:
	IllusionaryEntity(BSPMap& map);

	virtual rayhitinfo_t RayCollides(collisionray_t ray) override;
};

