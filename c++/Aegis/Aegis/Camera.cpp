#include "Camera.h"

#include <math.h>

#include <stdio.h>
#include <string.h>

#include "mathutils.h"

void Camera::ReconstructMatrices()
{
	vec3_t correctedrot = { rotation.x, rotation.y + 90 * DEG2RAD, rotation.z };
	rot = Quaternion::FromAngle(correctedrot);
	invrot = Quaternion::FromAngle(correctedrot * -1.0);

	matrix = rot.toMat();

	Mat3x4 mat;

	inv.val[0][0] = mat.val[0][0]; inv.val[0][1] = mat.val[0][1]; inv.val[0][2] = mat.val[0][2]; inv.val[0][3] = mat.val[0][3];
	inv.val[0][1] = mat.val[1][0]; inv.val[1][1] = mat.val[1][1]; inv.val[1][2] = mat.val[1][2]; inv.val[1][3] = mat.val[1][3];
	inv.val[0][2] = mat.val[2][0]; inv.val[2][1] = mat.val[2][1]; inv.val[2][2] = mat.val[2][2]; inv.val[2][3] = mat.val[2][3];

	/*inv = invrot.toMat();
	
	Mat3x4 invpos = Mat3x4::getIdentity();
	invpos.val[0][3] = -position.x;
	invpos.val[1][3] = -position.y;
	invpos.val[2][3] = -position.z;
	
	forward = { 0, 0, -1 };
	forward = matrix * forward;
	forward.x = -forward.x;
	up = { -1, 0, 0 };
	up = matrix * up;
	up.x = -up.x;
	right = { 0, -1, 0 };
	right = matrix * right;
	right.x = -right.x;

	matrix.val[0][3] = position.x;
	matrix.val[1][3] = position.y;
	matrix.val[2][3] = position.z;

	inv = inv * invpos;
	
	hfov = vfov * aspect;

	maxydir = tanf(vfov / 2.0 * DEG2RAD);
	maxxdir = maxydir * aspect;*/
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