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

void PlayerEntity::Think(float deltatime)
{
	float targetyaw;
	float targetvel;
	float yawaccel;

	vec3_t up;
	vec3_t right;
	vec3_t forward;
	vec3_t headpos;
	float dist;

	vec3_t rdir;
	vec3_t rpos;
	float t;

	vec3_t intersect;
	vec2_t local;

	up = { 0, 0, 1 };
	up = Quaternion::FromAngle(rotation).toMat() * up;

	headpos.x = studio.boneTransforms[1].val[0][3];
	headpos.y = studio.boneTransforms[1].val[1][3];
	headpos.z = studio.boneTransforms[1].val[2][3];

	dist = DotProduct(up, position + headpos);

	rdir = Game::GetGame().camera.DirFromScreen(Game::GetGame().cursorpos);
	rpos = Game::GetGame().camera.position;

	t = (dist - DotProduct(up, rpos)) / DotProduct(up, rdir);
	intersect = rdir * t + rpos;
	intersect = intersect - headpos - position;

	right = { 1, 0, 0 };
	right = Quaternion::FromAngle(rotation).toMat() * right;

	forward = { 0, 1, 0 };
	forward = Quaternion::FromAngle(rotation).toMat() * forward;

	local.x = DotProduct(right, intersect);
	local.y = DotProduct(forward, intersect);

	targetyaw = atan2f(local.y, local.x) + (180.0 * DEG2RAD);

	if (targetyaw - lookdir > M_PI)
		targetyaw -= M_PI * 2;

	if (lookdir - targetyaw > M_PI)
		targetyaw += M_PI * 2;

	targetvel = targetyaw - lookdir;
	yawaccel = targetvel - yawvel;

	if (yawaccel > 1.5 * ENGINE_TICKDUR)
		yawaccel = 1.5 * ENGINE_TICKDUR;
	if (yawaccel < -1.5 * ENGINE_TICKDUR)
		yawaccel = -1.5 * ENGINE_TICKDUR;

	yawvel += yawaccel;
	yawvel -= (yawvel * 0.9 * ENGINE_TICKDUR); // Friction

	if (yawvel > 3.0 * ENGINE_TICKDUR)
		yawvel = 3.0 * ENGINE_TICKDUR;
	if (yawvel < -3.0 * ENGINE_TICKDUR)
		yawvel = -3.0 * ENGINE_TICKDUR;

	lookdir += yawvel;
	
	while (lookdir > M_PI * 2)
		lookdir -= M_PI * 2;

	while (lookdir < 0)
		lookdir += M_PI * 2;

	if (lookdir - lastlookdir > M_PI) // So it doesn't try to lerp over the whole arc when crossing from 2 pi to 0
		lastlookdir += M_PI * 2;

	if (lastlookdir - lookdir > M_PI) // So it doesn't try to lerp over the whole arc when crossing from 2 pi to 0
		lastlookdir -= M_PI * 2;

	lastlastlookdir = lastlookdir;
	lastlookdir = lookdir;
}

void PlayerEntity::Render()
{
	studio.lastlasbonecontrollervalues[0] = studio.lastbonecontrollervalues[0] = studio.bonecontrollervalues[0] 
		= Lerp(lastlastlookdir, lastlookdir, Game::GetGame().tickinterp);

	studio.render();
}