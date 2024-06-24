#pragma once

#include "defs.h"

class Vector3
{
public:
	Vector3(float val[3]);
	Vector3(vec3_t v);

	void set(int which, float val);
	float get(int which);

	float length();
	Vector3 normalize();
	Vector3 normalized();

	static float dot(Vector3 a, Vector3 b);
	static Vector3 cross(Vector3 a, Vector3 b);
	static Vector3 reflect(Vector3 i, Vector3 n);

	Vector3 operator+(Vector3 a);
	Vector3 operator-(Vector3 a);
	Vector3 operator*(float a);
	Vector3 operator/(float a);
	Vector3 operator=(Vector3 a);
	bool operator==(Vector3 a);
private:
	float val[3];
	float vlength;
	float vnormalized[3];
	short accessflags;
};

