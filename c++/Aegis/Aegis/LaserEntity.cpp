#include "LaserEntity.h"

#include "Game.h"

#include "mathutils.h"
#include "Quaternion.h"

#include <stdio.h>

LaserEntity::LaserEntity(BSPMap& map) : BaseEntity(map)
{

}

void LaserEntity::Render()
{
	BeamFX::FracNoise(noisea, BEAM_NOISE_DIVISIONS - 1);
	BeamFX::SineNoise(sinenoisea, BEAM_NOISE_DIVISIONS - 1);
	BeamFX::FracNoise(noiseb, BEAM_NOISE_DIVISIONS - 1);
	BeamFX::SineNoise(sinenoiseb, BEAM_NOISE_DIVISIONS - 1);

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

	int nsegments = 64;
	float freq = 5;

	float div = 1.0 / (nsegments - 1);
	float length = Vector3Length(targetentity->position - position);
	if (length * div < width / 2.0 * 1.414) // Avoid corner overlapping (segments are too small)
	{
		nsegments = (int)(length / (width / 2.0 * 1.414)) + 1;

		if (nsegments < 2)
			nsegments = 2;
	}

	if (nsegments > BEAM_NOISE_DIVISIONS) // If nsegments is finer than our noise, it's a waste so we get rid of some.
		nsegments = BEAM_NOISE_DIVISIONS;

	div = 1.0 / (nsegments - 1);
	
	if (nsegments < 16)
	{
		nsegments = 16;
		div = 1.0 / (nsegments - 1);
	}
	length = (float) nsegments * 0.1;

	int noisestep = (int)((float)(BEAM_NOISE_DIVISIONS - 1) * div * 65536.0);
	float bright = 1.0;
	int index = 0;

	vec3_t dir = NormalizeVector3(targetentity->position - position);
	vec3_t beamright = CrossProduct({ 0, 0, 1 }, dir);
	vec3_t beamup = CrossProduct(dir, beamright);

	float diraxis[3] = { dir.x, dir.y, dir.z };
	Quaternion fracrot = Quaternion::AngleAxis(Game::GetGame().R_Random(0.0, 2 * M_PI), diraxis);
	Quaternion sinerot = Quaternion::AngleAxis(Game::GetGame().R_Random(0.0, 2 * M_PI), diraxis);

	float upaxis[3] = { beamup.x, beamup.y, beamup.z };
	Vector3 fracaxis = fracrot.toMat() * Vector3(upaxis);
	Vector3 sineaxis = sinerot.toMat() * Vector3(upaxis);

	vec3_t veca = { fracaxis.get(0), fracaxis.get(1), fracaxis.get(2) };
	vec3_t vecb = CrossProduct(veca, dir);

	vec3_t diff = targetentity->position - position;
	vec3_t step = diff * div;
	for (int i = 0; i < nsegments - 1; i++)
	{
		vec3_t pointa = step * i + position;
		vec3_t pointb = step * (i + 1) + position;

		vec3_t addfraca;
		vec3_t addfracb;
		vec3_t addsinea;
		vec3_t addsineb;

		addfraca = veca * noisea[index >> 16] * scale * 0.02 + vecb * noiseb[index >> 16] * scale * 0.02;
		addsinea = veca * sinenoisea[index >> 16] * scale * 0.2 + vecb * sinenoiseb[index >> 16] * scale * 0.2;
		index += noisestep;
		addfracb = veca * noisea[index >> 16] * scale * 0.02 + vecb * noiseb[index >> 16] * scale * 0.02;
		addsineb = veca * sinenoisea[index >> 16] * scale * 0.2 + vecb * sinenoiseb[index >> 16] * scale * 0.2;

		BeamFX::DrawLineSegment(pointa + addfraca + addsinea, pointb + addfracb + addsineb, 
			color, brightness, width, texture.texturenames[whichframe], texture.mhdr->maxheight, scrollspeed, camerapos);
	}
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