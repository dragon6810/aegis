#pragma once

#include "defs.h"
#include "Mat3x4.h"

class Camera
{
public:
	Camera() {}

	// Roll, Pitch, Yaw
	vec3_t rotation{};
	vec3_t position{};
	vec3_t forward{};

	float aspect;
	float vfov;
	bool ortho;
	float hfov;

	float maxxdir;
	float maxydir;

	Mat3x4 matrix;
	Mat3x4 inv;

	void ReconstructMatrices();

	// Screen coord in (0 - 1, 0 - 1);
	vec3_t DirFromScreen(vec2_t screencoord);
};

