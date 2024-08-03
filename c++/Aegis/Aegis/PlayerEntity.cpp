#include "PlayerEntity.h"

#include "Game.h"

#include "Quaternion.h"
#include "mathutils.h"

PlayerEntity::PlayerEntity(BSPMap& map) : BaseEntity(map)
{
	studio.map = &map;
	studio.Load((Game::GetGame().gamedir + "/models/tank/tank.mdl").c_str());

	lookdir = 0.0;
}

void PlayerEntity::Render()
{
	vec3_t up;
	vec3_t right;
	vec3_t forward;
	float dist;
	
	vec3_t rdir;
	vec3_t rpos;
	float t;

	vec3_t intersect;
	vec2_t local;

	up = { 0, 0, 1 };
	up = Quaternion::FromAngle(rotation).toMat() * up;

	dist = DotProduct(up, position) + studio.header->eyeposition.z;

	rdir = Game::GetGame().camera.DirFromScreen(Game::GetGame().cursorpos);
	rpos = Game::GetGame().camera.position;

	t = (dist - DotProduct(up, rpos)) / DotProduct(up, rdir);
	intersect = rdir * t + rpos;

	right = { 1, 0, 0 };
	right = Quaternion::FromAngle(rotation).toMat() * right;

	forward = { 0, 1, 0 };
	forward = Quaternion::FromAngle(rotation).toMat() * forward;

	local.x = DotProduct(right, intersect);
	local.y = DotProduct(forward, intersect);

	lookdir = atan2f(local.y, local.x);

	studio.lastlasbonecontrollervalues[0] = studio.lastbonecontrollervalues[0] = studio.bonecontrollervalues[0] = lookdir;

	studio.render();
}