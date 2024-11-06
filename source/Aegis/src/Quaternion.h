#pragma once

#include "Matrix.h"
#include "Vector.h"

#define PI 3.141596
#define DEG2RAD 2.0 * PI / 360.0
#define RAD2DEG 360.0 / 2.0 * PI

class Quaternion
{
public:
	static Vector3 ToRadians(Vector3 a);

	Quaternion();
	Quaternion(float w, float x, float y, float z);

	// w, x, y, z
	float q[4];

	// Rotation is expected to be in (Pitch Yaw Roll)
	static Quaternion FromEuler(Vector3 r);
	static Quaternion AxisAngle(Vector3 a, float r);

	void Normalize();
	Matrix4x4 ToMatrix4();

	Quaternion operator*(Quaternion b);
	float& operator[](int i);
};