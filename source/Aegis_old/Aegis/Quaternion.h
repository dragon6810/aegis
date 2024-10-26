#pragma once

#include "Mat3x4.h"

#include "defs.h"

#define RAD2DEG 57.2958
#define DEG2RAD 0.01745

struct Quaternion
{
	float q[4];

	static Quaternion AngleAxis(float theta, float axis[3]);
	static Quaternion FromAngle(float axis[3]);
	static Quaternion FromAngle(vec3_t angles);
	static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);
	vec3_t ToEuler();

	Mat3x4 toMat();

	Quaternion operator=(Quaternion a);
	bool operator==(Quaternion a);
	Quaternion operator*(Quaternion a);
	Quaternion operator*(float a);
};