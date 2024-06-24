#include "Camera.h"

#include "Quaternion.h"

void Camera::ReconstructMatrices()
{
	Quaternion rot = Quaternion::FromAngle(rotation);
	Quaternion invrot = Quaternion::FromAngle(rotation * -1.0);

	matrix = rot.toMat();
	inv = invrot.toMat();
	Mat3x4 invpos = Mat3x4::getIdentity();
	invpos.val[0][3] = -position.x;
	invpos.val[1][3] = -position.y;
	invpos.val[2][3] = -position.z;

	Vector3 forwardv = matrix * Vector3({ 0, 0, -1 });
	forward = { -forwardv.get(0), forwardv.get(1), forwardv.get(2) };

	matrix.val[0][3] = position.x;
	matrix.val[1][3] = position.y;
	matrix.val[2][3] = position.z;

	inv = inv * invpos;
}