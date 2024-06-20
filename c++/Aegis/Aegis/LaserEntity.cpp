#include "LaserEntity.h"

#include "mathutils.h"

#include <stdio.h>

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

	vec3_t direction = targetentity->position - position;
	vec3_t midpoint = (position + targetentity->position) * (1 / 2.0);
	vec3_t tocam = camerapos - midpoint;
	vec3_t perp = NormalizeVector3(CrossProduct(tocam, direction));

	vec3_t v1 = position + perp;
	vec3_t v2 = targetentity->position + perp;
	vec3_t v3 = targetentity->position - perp;
	vec3_t v4 = position - perp;

	glBegin(GL_QUADS);
	glVertex3f(v1.x ,v1.y, v1.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v3.x, v3.y, v3.z);
	glVertex3f(v4.x, v4.y, v4.z);
	glEnd();
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