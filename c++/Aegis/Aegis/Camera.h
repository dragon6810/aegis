#pragma once

#include "defs.h"
#include "Mat3x4.h"

class Camera
{
public:
	// Roll, Pitch, Yaw
	vec3_t rotation{};
	vec3_t position{};
	vec3_t forward{};

	Mat3x4 matrix;
	Mat3x4 inv;

	void ReconstructMatrices();
};

