#pragma once

#include "Vector3.h"

class Mat3x4
{
public:
	float val[3][4];

	Vector3 operator*(Vector3 a);
	Mat3x4 operator*(Mat3x4 b);
};

