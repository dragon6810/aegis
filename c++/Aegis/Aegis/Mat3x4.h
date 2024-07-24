#pragma once

#include <vector>

#include "defs.h"

class Mat3x4
{
public:
	Mat3x4();

	std::vector<std::vector<float>> val;

	static Mat3x4 getIdentity();

	vec3_t operator*(vec3_t a);
	Mat3x4 operator*(Mat3x4 b);
	Mat3x4 operator=(Mat3x4 m);
	bool operator==(Mat3x4 a);
};

