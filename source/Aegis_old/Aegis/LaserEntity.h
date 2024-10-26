#pragma once

#include "SpriteEntity.h"


#include "BeamFX.h"

class LaserEntity : public BaseEntity
{
public:
	LaserEntity(BSPMap& map);

	virtual void Render() override;

	std::string target;
	BaseEntity* targetentity = nullptr;

	std::string texturename = "sprites/laserbeam.spr";
	int whichframe = 0;
	SpriteEntity texture;

	float scale = 0;

	vec3_t color = { 0, 0, 0 };
	float brightness = 1.0;
	float scrollspeed = 0.0; // Textures per second
	float width = 25.5;
private:
	float noisea[BEAM_NOISE_DIVISIONS]{};
	float sinenoisea[BEAM_NOISE_DIVISIONS]{};
	float noiseb[BEAM_NOISE_DIVISIONS]{};
	float sinenoiseb[BEAM_NOISE_DIVISIONS]{};

	void FindTarget();
};

