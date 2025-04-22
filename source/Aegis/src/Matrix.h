#pragma once

#include "Vector.h"

#include "Matrix.h"

class Matrix4x4
{
public:
	Matrix4x4(void);

	float m[4][4];
	Vector3 v;

	Matrix4x4 operator*(Matrix4x4 a);
	Vector3 operator*(Vector3 v);
	float* operator[](int i);
};

class Matrix3x3
{
public:
	Matrix3x3(void);

	float m[3][3];
	Vector3 v;

	Vector3 SetColumn(Vector3 v, int column);

	Matrix3x3 operator*(Matrix3x3 a);
	Vector3 operator*(Vector3 v);
	float* operator[](int i);
};