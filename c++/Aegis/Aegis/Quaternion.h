#pragma once

#include "Mat3x4.h"

struct Quaternion
{
	float q[4];

	static Quaternion AngleAxis(float theta, float axis[3]);

	Mat3x4 toMat();

	Quaternion operator=(Quaternion a);
	bool operator==(Quaternion a);
	Quaternion operator*(Quaternion a);
};