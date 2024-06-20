#include "LaserEntity.h"

#include "Game.h"

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

	if (texture.mhdr == nullptr)
	{
		texture.SetMap(*map);
		texture.LoadTexture((char*) texturename.c_str());
	}

	vec3_t direction = targetentity->position - position;
	vec3_t midpoint = (position + targetentity->position) * (1 / 2.0);
	vec3_t tocam = camerapos - midpoint;
	vec3_t perp = NormalizeVector3(CrossProduct(tocam, direction));

	vec3_t v1 = position + perp * width;
	vec3_t v2 = targetentity->position + perp * width;
	vec3_t v3 = targetentity->position - perp * width;
	vec3_t v4 = position - perp * width;

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture.texturenames[whichframe]);

	switch (texture.mhdr->textureformat)
	{
	case SPRITE_TEXTURE_NORMAL:
		break;
	case SPRITE_TEXTURE_ADDITIVE:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	case SPRITE_TEXTURE_INDEXALPHA:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case SPRITE_TEXTURE_ALPHATEST:
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GEQUAL, 0.75f);
	default:
		break;
	}

	glColor3f(color.x * brightness, color.y * brightness, color.z * brightness);

	float numytex = Vector3Length(direction) / texture.mhdr->maxheight;
	float yoffs = Game::GetGame().Time() * scrollspeed;

	glBegin(GL_QUADS);
	glTexCoord2f(0.5, 0.0 + yoffs);
	glVertex3f(v1.x ,v1.y, v1.z);
	glTexCoord2f(0.5, numytex + yoffs);
	glVertex3f(v2.x, v2.y, v2.z);
	glTexCoord2f(0.0, numytex + yoffs);
	glVertex3f(v3.x, v3.y, v3.z);
	glTexCoord2f(0.0, 0.0 + yoffs);
	glVertex3f(v4.x, v4.y, v4.z);
	glEnd();

	glColor3f(1, 1, 1);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
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