#pragma once

#include <vector>

#include "defs.h"

class Mat3x4
{
public:
	Mat3x4();

	float val[4][4]; // FIX: Random memory stuff goes wrong when this array is 3x4, setting it to 4x4 for now

	static Mat3x4 getIdentity();

	vec3_t operator*(vec3_t a);
	Mat3x4 operator*(Mat3x4 b);
	Mat3x4 operator=(Mat3x4 m);
	bool operator==(Mat3x4 a);
};

