#pragma once

#include "Mat3x4.h"

struct Quaternion
{
	float q[4];

	static Quaternion AngleAxis(float theta, float axis[3]);
	static Quaternion FromAngle(float axis[3]);
	static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);

	Mat3x4 toMat();

	Quaternion operator=(Quaternion a);
	bool operator==(Quaternion a);
	Quaternion operator*(Quaternion a);
	Quaternion operator*(float a);
};