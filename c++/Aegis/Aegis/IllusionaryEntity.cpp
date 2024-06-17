#include "IllusionaryEntity.h"

IllusionaryEntity::IllusionaryEntity(BSPMap& map) : WallEntity(map)
{

}

rayhitinfo_t IllusionaryEntity::RayCollides(collisionray_t ray)
{
	return { false, {0.0, 0.0, 0.0} };
}