#pragma once

class Matrix4x4
{
public:
	Matrix4x4();

	float m[4][4];

	Matrix4x4 operator*(Matrix4x4 a);
	float* operator[](int i);
};