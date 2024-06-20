#include "LaserEntity.h"

#include "Game.h"

#include "mathutils.h"

#include <stdio.h>

#include "BeamFX.h"

LaserEntity::LaserEntity(BSPMap& map) : BaseEntity(map)
{

}

void LaserEntity::Render()
{
	if (targetentity == nullptr)
		FindTarget();

	if (targetentity == nullptr)
	{
		printf("*WARNING* Laser Entity %s can't find target %s!", targetname.c_str(), target.c_str());
		return;
	}

	if (texture.mhdr == nullptr)
	{
		texture.SetMap(*map);
		texture.LoadTexture((char*) texturename.c_str());
	}

	BeamFX::DrawLineSegment(position, targetentity->position, color, brightness, width, texture.texturenames[whichframe], texture.mhdr->maxheight, scrollspeed, camerapos);
}

void LaserEntity::FindTarget()
{
	for (int i = 0; i < map->entities.size(); i++)
	{
		if (target == map->entities[i]->targetname)
		{
			targetentity = &(*map->entities[i]);
			break;
		}
	}
}