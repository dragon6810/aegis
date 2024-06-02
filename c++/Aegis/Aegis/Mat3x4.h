#pragma once

#include "Vector3.h"

class Mat3x4
{
public:
	float val[3][4];

	static Mat3x4 getIdentity();

	Vector3 operator*(Vector3 a);
	Mat3x4 operator*(Mat3x4 b);
	bool operator==(Mat3x4 a);
};

