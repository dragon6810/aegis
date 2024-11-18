#pragma once

#include "Vector.h"

#include "Matrix.h"

class Matrix4x4
{
public:
	Matrix4x4();

	float m[4][4];

	Vector3 v;

	Matrix4x4 operator*(Matrix4x4 a);
	Vector3 operator*(Vector3 v);
	float* operator[](int i);
};