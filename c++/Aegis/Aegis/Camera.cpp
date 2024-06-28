#include "Camera.h"

#include <math.h>

#include <stdio.h>

#include "mathutils.h"
#include "Quaternion.h"

void Camera::ReconstructMatrices()
{
	vec3_t correctedrot = { rotation.x, rotation.y + 90 * DEG2RAD, rotation.z };
;	Quaternion rot = Quaternion::FromAngle(correctedrot);
	Quaternion invrot = Quaternion::FromAngle(correctedrot * -1.0);

	matrix = rot.toMat();
	inv = invrot.toMat();
	Mat3x4 invpos = Mat3x4::getIdentity();
	invpos.val[0][3] = -position.x;
	invpos.val[1][3] = -position.y;
	invpos.val[2][3] = -position.z;

	Vector3 forwardv = matrix * Vector3({ 0, 0, -1 });
	forward = { -forwardv.get(0), forwardv.get(1), forwardv.get(2) };
	Vector3 upv = matrix * Vector3({ -1, 0, 0 });
	up = { -upv.get(0), upv.get(1), upv.get(2) };
	Vector3 rightv = matrix * Vector3({ 0, -1, 0 });
	right = { -rightv.get(0), rightv.get(1), rightv.get(2) };

	matrix.val[0][3] = position.x;
	matrix.val[1][3] = position.y;
	matrix.val[2][3] = position.z;

	inv = inv * invpos;
	
	hfov = vfov * aspect;

	maxydir = tanf(vfov / 2.0 * DEG2RAD);
	maxxdir = maxydir * aspect;
}

vec3_t Camera::DirFromScreen(vec2_t screencoord)
{
	float x = Lerp(-maxxdir, maxxdir, screencoord.x);
	float y = Lerp(maxydir, -maxydir, screencoord.y);
	float z = 1.0;

	vec3_t dir = right * x + up * y + forward * z;
	dir = NormalizeVector3(dir);

	return dir;
}